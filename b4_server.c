#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NUM_QUESTIONS 10

typedef struct {
    char question[BUFFER_SIZE];
    char options[4][BUFFER_SIZE];
    int correct_option;
} Question;

Question questions[NUM_QUESTIONS] = {
    {"What is the capital of France?", {"a. Berlin", "b. Madrid", "c. Paris", "d. Rome"}, 2},
    {"Which planet is known as the Red Planet?", {"a. Earth", "b. Mars", "c. Jupiter", "d. Saturn"}, 1},
    {"What is the largest ocean on Earth?", {"a. Atlantic Ocean", "b. Indian Ocean", "c. Arctic Ocean", "d. Pacific Ocean"}, 3},
    {"Who wrote 'To Kill a Mockingbird'?", {"a. Harper Lee", "b. Mark Twain", "c. J.K. Rowling", "d. Ernest Hemingway"}, 0},
    {"What is the chemical symbol for water?", {"a. H2O", "b. O2", "c. CO2", "d. NaCl"}, 0},
    {"Which element has the atomic number 1?", {"a. Helium", "b. Oxygen", "c. Hydrogen", "d. Carbon"}, 2},
    {"Who painted the Mona Lisa?", {"a. Vincent van Gogh", "b. Pablo Picasso", "c. Leonardo da Vinci", "d. Claude Monet"}, 2},
    {"What is the smallest prime number?", {"a. 0", "b. 1", "c. 2", "d. 3"}, 2},
    {"Which country is known as the Land of the Rising Sun?", {"a. China", "b. South Korea", "c. Japan", "d. Thailand"}, 2},
    {"What is the hardest natural substance on Earth?", {"a. Gold", "b. Iron", "c. Diamond", "d. Silver"}, 2}
};

void handle_client(int client_socket) {
    int score = 0;
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < NUM_QUESTIONS; i++) {
        // Sử dụng snprintf để định dạng câu hỏi và các lựa chọn
        snprintf(buffer, sizeof(buffer), "%s\n%s\n%s\n%s\n%s\n",
                 questions[i].question,
                 questions[i].options[0],
                 questions[i].options[1],
                 questions[i].options[2],
                 questions[i].options[3]);
        send(client_socket, buffer, strlen(buffer), 0);

        // Nhận câu trả lời từ client
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        char answer = buffer[0];

        // Kiểm tra câu trả lời
        if (answer == 'a' + questions[i].correct_option) {
            score++;
        }//answer == a(ascci) + correct_option
    }

    // Sử dụng snprintf để định dạng điểm số
    snprintf(buffer, sizeof(buffer), "Your score: %d/%d\n", score, NUM_QUESTIONS);
    send(client_socket, buffer, strlen(buffer), 0);

    close(client_socket);
    exit(0);
}

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}//thu hồi tiến trình con đã kết thúc

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Gán địa chỉ cho socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


// Thiết lập handler để thu gom tiến trình zombie
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // Đặt hàm xử lý tín hiệu là sigchld_handler
    sigemptyset(&sa.sa_mask); // Xóa tất cả các tín hiệu khỏi tập tín hiệu sa_mask
    sa.sa_flags = SA_RESTART; // Đặt cờ SA_RESTART để tự động khởi động lại các hàm bị gián đoạn bởi tín hiệu
    if (sigaction(SIGCHLD, &sa, NULL) == -1) { // Đăng ký handler cho tín hiệu SIGCHLD
        perror("sigaction"); // In ra thông báo lỗi nếu sigaction thất bại
        exit(EXIT_FAILURE); // Thoát chương trình với mã lỗi
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        if (fork() == 0) {
            close(server_fd);
            handle_client(client_socket);
        } else {
            close(client_socket);
        }
    }

    return 0;
}