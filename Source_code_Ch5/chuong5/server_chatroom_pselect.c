#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

volatile sig_atomic_t stop = 0;

void handle_signal(int signal) {
    stop = 1;
}

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    struct timespec timeout;
    sigset_t sigmask;

    signal(SIGINT, handle_signal);
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("Server đang chạy trên cổng %d...\n", PORT);

    while (!stop) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        int max_fd = server_fd;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) max_fd = client_sockets[i];
            }
        }

        timeout.tv_sec = 5;
        timeout.tv_nsec = 0;
        int activity = pselect(max_fd + 1, &read_fds, NULL, NULL, &timeout, &sigmask);

        if (activity > 0) {
            if (FD_ISSET(server_fd, &read_fds)) {
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
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &read_fds)) {
                    int valread = read(client_sockets[i], buffer, BUFFER_SIZE);
                    if (valread == 0) {
                        close(client_sockets[i]);
                        client_sockets[i] = 0;
                        printf("Client %d đã rời phòng chat.\n", i);
                    } else {
                        buffer[valread] = '\0';
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_sockets[j] > 0 && j != i) {
                                send(client_sockets[j], buffer, strlen(buffer), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}