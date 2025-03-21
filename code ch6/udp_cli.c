#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define XOR_KEY 0x55
#define TOKEN "SERVER"

void xor_cipher(char *data, char key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= key;
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    struct timeval timeout;

    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }

    // Sử dụng connect() để tối ưu hóa giao tiếp UDP
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Client kết nối tới server %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Nhập tin nhắn để gửi:\n");

    // Vòng lặp gửi tin nhắn và nhận phản hồi
    while (1) {
        // Đọc tin nhắn từ bàn phím
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Mã hóa tin nhắn trước khi gửi
        xor_cipher(buffer, XOR_KEY);
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("send failed");
        }

        // Chờ phản hồi từ server với timeout
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select error");
            break;
        } else if (activity == 0) {
            printf("Timeout! Không nhận được phản hồi từ server.\n");
            continue;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n < 0) {
                perror("recv failed");
                continue;
            }
            buffer[n] = '\0';
            // Giải mã phản hồi
            xor_cipher(buffer, XOR_KEY);
            // Xác minh danh tính của server bằng cách so sánh token
            if (memcmp(buffer, TOKEN, strlen(TOKEN)) != 0) {
                printf("Xác minh server thất bại!\n");
                continue;
            }
            printf("Phản hồi từ server: %s\n", buffer);
        }
    }
    close(sockfd);
    return 0;
}
