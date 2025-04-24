#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void send_response(int client_sock, const char *status, const char *content_type, const char *body) {
    char header[BUFFER_SIZE];
    sprintf(header,
            "HTTP/1.1 %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n\r\n",
            status, content_type, body ? strlen(body) : 0);
    send(client_sock, header, strlen(header), 0);
    if (body) send(client_sock, body, strlen(body), 0);
}

void handle_get(int client_sock, const char *path) {
    if (strcmp(path, "/") == 0) path = "/index.html";
    char file_path[1024];
    sprintf(file_path, ".%s", path);

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        send_response(client_sock, "404 Not Found", "text/plain", "File not found");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *body = malloc(length + 1);
    fread(body, 1, length, fp);
    body[length] = '\0';

    const char *content_type = strstr(file_path, ".css") ? "text/css" : "text/html";
    send_response(client_sock, "200 OK", content_type, body);

    fclose(fp);
    free(body);
}

void handle_post(int client_sock, const char *body) {
    FILE *fp = fopen("post_data.txt", "w");
    if (fp) {
        fprintf(fp, "%s\n", body);
        fclose(fp);
    }
    send_response(client_sock, "200 OK", "text/html", "<html><body>POST data received</body></html>");
}

void handle_head(int client_sock, const char *path) {
    if (strcmp(path, "/") == 0) path = "/index.html";
    char file_path[1024];
    sprintf(file_path, ".%s", path);

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        send_response(client_sock, "404 Not Found", "text/plain", NULL);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fclose(fp);

    char header[BUFFER_SIZE];
    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n", length);
    send(client_sock, header, strlen(header), 0);
}

void handle_put(int client_sock, const char *body) {
    printf("PUT data: %s\n", body);
    send_response(client_sock, "200 OK", "text/plain", "PUT method processed (mock)");
}

void handle_delete(int client_sock, const char *path) {
    printf("DELETE request for: %s\n", path);
    send_response(client_sock, "200 OK", "text/plain", "DELETE method processed (mock)");
}

void handle_request(int client_sock) {
    char buffer[BUFFER_SIZE];
    int received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (received <= 0) return;

    buffer[received] = '\0';

    char method[8], path[1024];
    sscanf(buffer, "%s %s", method, path);

    char *body = strstr(buffer, "\r\n\r\n");
    if (body) body += 4;

    if (strcmp(method, "GET") == 0) {
        handle_get(client_sock, path);
    } else if (strcmp(method, "POST") == 0) {
        handle_post(client_sock, body);
    } else if (strcmp(method, "HEAD") == 0) {
        handle_head(client_sock, path);
    } else if (strcmp(method, "PUT") == 0) {
        handle_put(client_sock, body);
    } else if (strcmp(method, "DELETE") == 0) {
        handle_delete(client_sock, path);
    } else {
        send_response(client_sock, "405 Method Not Allowed", "text/plain", "Unsupported Method");
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    listen(server_sock, 10);
    printf("HTTP Server running on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &sin_len);
        if (client_sock < 0) continue;
        if (fork() == 0) {
            close(server_sock);
            handle_request(client_sock);
            close(client_sock);
            exit(0);
        }
        close(client_sock);
    }

    return 0;
}
