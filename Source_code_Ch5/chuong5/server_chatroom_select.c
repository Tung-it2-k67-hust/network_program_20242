#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];

void broadcast_message(int sender_fd, char *message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_fd = client_sockets[i];
        if (client_fd != sender_fd && client_fd > 0) {
            send(client_fd, message, strlen(message), 0);
        }
    }
}

int main() {
    int server_fd, new_socket, max_sd, activity, sd;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // Tạo socket server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Gán socket
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // Lắng nghe kết nối
    listen(server_fd, MAX_CLIENTS);

    // Khởi tạo danh sách client
    for (int i = 0; i < MAX_CLIENTS; i++) client_sockets[i] = 0;

    printf("Server đang chạy trên cổng %d...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Thêm các socket client vào tập hợp kiểm tra
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Chờ sự kiện trên socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select() error");
        }

        // Kiểm tra kết nối mới
        if (FD_ISSET(server_fd, &readfds)) {
            int addrlen = sizeof(address);
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

            printf("Client mới kết nối, socket fd: %d\n", new_socket);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Kiểm tra dữ liệu từ client
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    close(sd);
                    client_sockets[i] = 0;
                    printf("Client %d đã rời phòng chat.\n", sd);
                } else {
                    buffer[valread] = '\0';
                    broadcast_message(sd, buffer);
                }
            }
        }
    }

    return 0;
}
