#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    struct sockaddr_in sa;
    const char *ip_address = "192.168.1.1";
    int port = 8080;

    // avoid unwanted data in the structure
    memset(&sa, 0, sizeof(sa));

    // ho dia chi (IPv4)
    sa.sin_family = AF_INET;

    // host to network byte order
    sa.sin_port = htons(port);

    // Set the IP address
    if (inet_pton(AF_INET, ip_address, &sa.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    // Print the initialized sockaddr_in structure
    printf("Initialized sockaddr_in:\n");
    printf("Family: %d\n", sa.sin_family);
    printf("Port: %d\n", ntohs(sa.sin_port));
    printf("Address: %s\n", ip_address);

    return 0;
}
// struct sockaddr_in {
//     short int sin_family; // Họ địa chỉ (AF_INET cho IPv4)
//     unsigned short int sin_port; // Số cổng (sử dụng htons() để gán)
//     struct in_addr sin_addr; // Địa chỉ Internet (cấu trúc in_addr)
//     unsigned char sin_zero[8]; // Đệm để làm cho struct có kích thước giống
//     sockaddr
//     };