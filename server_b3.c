#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Hàm xử lý giao tiếp với client
void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE] = {0};
    char client_ip[INET_ADDRSTRLEN];

    // Đọc thông điệp từ client
    read(client_socket, buffer, BUFFER_SIZE);
    printf("Thông điệp từ client: %s\n", buffer);

    // Chuyển đổi địa chỉ client sang chuỗi
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Địa chỉ IP của client: %s\n", client_ip);

    // Gửi phản hồi tới client
    const char *response = "Hello from server";
    send(client_socket, response, strlen(response), 0);
    printf("Đã gửi phản hồi tới client\n");

    // Đóng socket của client
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(address);

    // Tạo socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Tạo socket thất bại");
        exit(EXIT_FAILURE);
    }

    // Gán địa chỉ và cổng cho socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Gán địa chỉ thất bại");
        exit(EXIT_FAILURE);
    }

    // Bắt đầu lắng nghe kết nối
    if (listen(server_fd, 3) < 0) {
        perror("Lắng nghe thất bại");
        exit(EXIT_FAILURE);
    }

    printf("Server đang lắng nghe trên cổng %d\n", PORT);

    // Chấp nhận kết nối từ client
    while ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) >= 0) {
        handle_client(client_socket, client_addr);
    }

    if (client_socket < 0) {
        perror("Chấp nhận kết nối thất bại");
        exit(EXIT_FAILURE);
    }

    // Đóng socket của server
    close(server_fd);

    return 0;
}