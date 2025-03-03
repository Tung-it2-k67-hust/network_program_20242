#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char server_ip[INET_ADDRSTRLEN];

    // Nhập địa chỉ IP của server từ người dùng
    printf("Nhập địa chỉ IP của server: ");
    scanf("%s", server_ip);

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Tạo socket thất bại \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IPv4 và IPv6 từ dạng text sang dạng nhị phân
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\nĐịa chỉ không hợp lệ/ Địa chỉ không được hỗ trợ \n");
        return -1;
    }

    // Kết nối tới server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nKết nối thất bại \n");
        return -1;
    }

    // Gửi thông điệp tới server
    const char *message = "Hello from client";
    send(sock, message, strlen(message), 0);
    printf("Đã gửi thông điệp tới server\n");

    // Đọc phản hồi từ server
    read(sock, buffer, BUFFER_SIZE);
    printf("Thông điệp từ server: %s\n", buffer);

    // Đóng socket
    close(sock);

    return 0;
}