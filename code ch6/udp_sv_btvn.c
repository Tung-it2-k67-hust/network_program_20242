#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define XOR_KEY 0x55
#define TOKEN "SERVER"

typedef struct {
    struct sockaddr_in addr;
    socklen_t addr_len;
} client_t;

void xor_cipher(char *data, char key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= key;
    }
}

int client_exists(client_t *clients, int count, struct sockaddr_in *addr) {
    for (int i = 0; i < count; i++) {
        if (clients[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].addr.sin_port == addr->sin_port) {
            return 1;
        }
    }
    return 0;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    client_t clients[MAX_CLIENTS];
    int client_count = 0;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int maxfd;
    struct timeval timeout;

    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Liên kết socket với địa chỉ IP và cổng
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server đang chạy trên cổng %d...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfd = sockfd;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select error");
            break;
        } else if (activity == 0) {
            // Timeout: có thể thực hiện các nhiệm vụ định kỳ tại đây
            continue;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            socklen_t len = sizeof(client_addr);
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &len);
            if (n < 0) {
                perror("recvfrom error");
                continue;
            }
            buffer[n] = '\0';

            // Giải mã thông điệp nhận được
            xor_cipher(buffer, XOR_KEY);
            printf("Nhận từ client %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

            // Kiểm tra và thêm client nếu chưa có
            if (!client_exists(clients, client_count, &client_addr) && client_count < MAX_CLIENTS) {
                clients[client_count].addr = client_addr;
                clients[client_count].addr_len = len;
                client_count++;
            }

            // Chuẩn bị phản hồi: thêm TOKEN vào đầu thông điệp
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "%s: %s", TOKEN, buffer);
            // Mã hóa phản hồi
            xor_cipher(response, XOR_KEY);

            // Gửi phản hồi đến tất cả client đã biết
            for (int i = 0; i < client_count; i++) {
                sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&clients[i].addr, clients[i].addr_len);
            }
        }
    }
    close(sockfd);
    return 0;
}
