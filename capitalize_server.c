#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;//thong bao tao thanh cong
    struct sockaddr_in address;//dia chi cua server
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Ràng buộc địa chỉ và cổng
    address.sin_family = AF_INET;//dia chi ip4
    address.sin_addr.s_addr = INADDR_ANY;//moi dia chi ip
    address.sin_port = htons(PORT);//host to etwork byte

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {//gan dia chi va cong
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    /*(struct sockaddr *) chuyển đổi con trỏ từ một kiểu cụ thể (ví dụ: struct sockaddr_in) sang kiểu struct sockaddr.
    Điều này là cần thiết vì các hàm mạng như bind(), connect(), và accept() yêu cầu tham số địa chỉ phải là kiểu struct sockaddr *.
    */
   if(listen(server_fd,3)<0){//cho phep 3 ket noi
         perror("Listen failed");
         exit(EXIT_FAILURE);
   }
   printf("Server listening on port %d\n", PORT);
   if((new_socket = accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen))<0){
         perror("Accept failed");
         exit(EXIT_FAILURE);
   }
   //struct sockaddr* va socklen_t* can su dung voi cac ham kia

read(new_socket, buffer, BUFFER_SIZE);
    printf("Tin nhan tu client: %s\n", buffer);

    // Chuyển đổi thành chữ hoa
    for (int i = 0; buffer[i]; i++) {
        buffer[i] = toupper(buffer[i]);
    }

    // Gửi lại cho client
    send(new_socket, buffer, strlen(buffer), 0);//send buffer toi cong new_socket
    printf("Capitalized message sent to client\n");

    // Đóng socket
    close(new_socket);
    close(server_fd);

    return 0;
    
}