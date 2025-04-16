#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 16

int game_board[3][3];
int move_count = 0;

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

int check_winner(int b[3][3]) {
    // Các dòng, cột, chéo
    for (int i = 0; i < 3; i++) {
        if (b[i][0] && b[i][0] == b[i][1] && b[i][1] == b[i][2]) return b[i][0];
        if (b[0][i] && b[0][i] == b[1][i] && b[1][i] == b[2][i]) return b[0][i];
    }
    if (b[0][0] && b[0][0] == b[1][1] && b[1][1] == b[2][2]) return b[0][0];
    if (b[0][2] && b[0][2] == b[1][1] && b[1][1] == b[2][0]) return b[0][2];
    if (move_count == 9) return 3; // Hòa
    return 0; // Chưa xong
}

int main() {
    int server_fd, client1, client2;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    unsigned char buffer[BUFFER_SIZE];
    int current_player = PLAYER_X;

    // Khởi tạo board
    memset(game_board, 0, sizeof(game_board));

    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 2);
    printf("Server listening on port %d...\n", PORT);

    // Chấp nhận hai client
    client1 = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Player X connected.\n");
    client2 = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Player O connected.\n");

    // Vòng chơi
    while (1) {
        int turn_sock = (current_player == PLAYER_X ? client1 : client2);
        int other1 = client1, other2 = client2;

        // Gửi TURN_NOTIFICATION
        buffer[0] = 0x05;
        send(turn_sock, buffer, 1, 0);

        // Nhận MOVE
        recv(turn_sock, buffer, BUFFER_SIZE, 0);
        int row = buffer[1], col = buffer[2];
        if (row < 0 || row > 2 || col < 0 || col > 2 || game_board[row][col] != 0) {
            // Yêu cầu thử lại
            continue;
        }

        // Cập nhật board
        game_board[row][col] = current_player;
        move_count++;
        print_board(game_board);

        // Gửi STATE_UPDATE
        buffer[0] = 0x03;
        for (int i = 0, idx = 1; i < 3; i++)
            for (int j = 0; j < 3; j++)
                buffer[idx++] = game_board[i][j];
        send(client1, buffer, 10, 0);
        send(client2, buffer, 10, 0);

        // Kiểm tra kết thúc
        int result = check_winner(game_board);
        if (result) {
            buffer[0] = 0x04;
            buffer[1] = (unsigned char)result;
            send(client1, buffer, 2, 0);
            send(client2, buffer, 2, 0);
            break;
        }

        // Đổi lượt
        current_player = (current_player == PLAYER_X ? PLAYER_O : PLAYER_X);
    }

    close(client1);
    close(client2);
    close(server_fd);
    return 0;
}