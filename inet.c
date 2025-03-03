// Bài 2: Các hàm Chuyển đổi Địa chỉ
// 2.1 inet_aton và inet_ntoa:
// • inet_aton: Chuyển đổi một chuỗi (ví dụ: "192.168.1.1") thành struct in_addr.
// • inet_ntoa: Chuyển đổi một in_addr thành chuỗi địa chỉ IP dễ đọc.
// 2.2 inet_pton và inet_ntop:
// • inet_pton: Chuyển đổi địa chỉ IP dạng văn bản (IPv4/IPv6) sang dạng nhị
// phân.
// • inet_ntop: Chuyển đổi địa chỉ IP dạng nhị phân (IPv4/IPv6) về dạng văn bản
// dễ đọc.
// Yêu cầu 2: Viết một chương trình thực hiện:
// - Nhập địa chỉ IP dưới dạng chuỗi từ người dùng.
// - Sử dụng inet_pton để chuyển địa chỉ sang dạng nhị phân.
// - Chuyển địa chỉ nhị phân về dạng văn bản dễ đọc bằng inet_ntop và in ra.
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    struct in_addr addr;
    char ip_a_bin[16];
    printf("Enter an IP address: ");
    scanf("%s", ip_a_bin);
    if (inet_pton(AF_INET, ip_a_bin, &addr) <= 0) {
        perror("inet_pton");
        return 1;
    }
    char ip_a_read[16];
    if (inet_ntop(AF_INET, &addr, ip_a_read, sizeof(ip_a_read)) == NULL) {
        perror("inet_ntop");
        return 1;
    }
    printf("IP address nhi phan: %s\n", ip_a_bin);
    printf("IP address doc duoc: %s\n", ip_a_read);
    return 0;
}