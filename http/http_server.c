// server.c
// Simple HTTP Server in C using fork()
// Yêu cầu: phục vụ tệp HTML, CSS; xử lý 404, 405

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/wait.h>  // Added to support waitpid

#define PORT 8080
#define BUF_SIZE 4096

// Xử lý SIGCHLD để tránh zombie
void handle_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Xác định MIME type theo phần mở rộng
const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "text/plain";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css")  == 0) return "text/css";
    return "application/octet-stream";
}

void serve_client(int client_fd) {
    char buf[BUF_SIZE];
    int len = recv(client_fd, buf, sizeof(buf)-1, 0);
    if (len <= 0) {
        close(client_fd);
        exit(0);
    }
    buf[len] = '\0';

    // Lấy dòng đầu: METHOD URI HTTP/1.x
    char method[16], uri[256];
    sscanf(buf, "%s %s", method, uri);

    // Chỉ hỗ trợ GET
    if (strcmp(method, "GET") != 0) {
        const char *resp405 = 
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, resp405, strlen(resp405), 0);
        close(client_fd);
        exit(0);
    }

    // Xử lý URI thành đường dẫn file
    char path[512] = ".";
    if (strcmp(uri, "/") == 0) {
        strcat(path, "/index.html");
    } else {
        strcat(path, uri);
    }

    // Mở file
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        const char *resp404 =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, resp404, strlen(resp404), 0);
        close(client_fd);
        exit(0);
    }

    // Lấy kích thước file
    struct stat st;
    fstat(fd, &st);

    // Gửi header
    const char *mime = get_mime_type(path);
    char header[BUF_SIZE];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        mime, st.st_size);
    send(client_fd, header, header_len, 0);

    // Gửi body
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        send(client_fd, buf, r, 0);
    }

    close(fd);
    close(client_fd);
    exit(0);
}

int main() {
    signal(SIGCHLD, handle_sigchld);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server running on port %d\n", PORT);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
        } else if (pid == 0) {
            // Con xử lý request
            close(server_fd);
            serve_client(client_fd);
        } else {
            // Cha tiếp tục
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
