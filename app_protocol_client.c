#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 16
#define PLAYER_X 1
#define PLAYER_O 2
int board[3][3];

void print_board(int b[3][3]) {
    printf("Current board:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char c = (b[i][j] == 1 ? 'X' : (b[i][j] == 2 ? 'O' : '.'));
            printf(" %c", c);
        }
        printf("\n");
    }
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    unsigned char buffer[BUFFER_SIZE];
    int running = 1;

    // Tạo kết nối
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server %s:%d\n", SERVER_IP, PORT);

    while (running) {
        int len = recv(sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) break;
        unsigned char type = buffer[0];

        if (type == 0x05) {
            // Đến lượt
            int row, col;
            print_board(board);
            printf("Your move (row col): ");
            scanf("%d %d", &row, &col);
            buffer[0] = 0x02;
            buffer[1] = (unsigned char)row;
            buffer[2] = (unsigned char)col;
            send(sock, buffer, 3, 0);

        } else if (type == 0x03) {
            // Cập nhật trạng thái
            for (int i = 0, idx = 1; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    board[i][j] = buffer[idx++];
            print_board(board);

        } else if (type == 0x04) {
            // Kết quả
            int res = buffer[1];
            if (res == 3) printf("Game ended in a draw!\n");
            else if ((res == 1 && board[0][0] == 1) || (res == 2 && board[0][0] == 2))
                printf("You win!\n");
            else printf("You lose!\n");
            running = 0;
        }
    }

    close(sock);
    return 0;
}