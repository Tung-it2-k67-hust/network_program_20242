#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    struct pollfd fds[MAX_CLIENTS + 1];
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    for (int i = 1; i <= MAX_CLIENTS; i++) fds[i].fd = -1;

    printf("Server đang chạy trên cổng %d...
", PORT);

    while (1) {
        int activity = poll(fds, MAX_CLIENTS + 1, -1);
        if (activity > 0) {
            if (fds[0].revents & POLLIN) {
                int addrlen = sizeof(address);
                new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                printf("Client mới kết nối, socket fd: %d
", new_socket);
                for (int i = 1; i <= MAX_CLIENTS; i++) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = new_socket;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
            }
            for (int i = 1; i <= MAX_CLIENTS; i++) {
                if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                    int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                    if (valread == 0) {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        printf("Client %d đã rời phòng chat.
", i);
                    } else {
                        buffer[valread] = '\0';
                        for (int j = 1; j <= MAX_CLIENTS; j++) {
                            if (fds[j].fd != -1 && j != i) {
                                send(fds[j].fd, buffer, strlen(buffer), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}