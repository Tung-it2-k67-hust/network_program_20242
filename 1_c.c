#include <stdio.h>
#include <stdlib.h>

typedef struct student {
    char name[20];
    int eng;
    int math;
    int phys;
} STUDENT;

STUDENT data[] = {
    {"Tuan", 82, 72, 58},
    {"Nam", 77, 82, 79},
    {"Khanh", 52, 62, 39},
    {"Phuong", 61, 82, 88}
};

int main() {
    STUDENT *p; // Khai báo con trỏ p

    // Gán địa chỉ của mảng data cho con trỏ p
    p = data;

    // Duyệt qua mảng data bằng con trỏ p
    for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        printf("Student %d: Name: %s, English: %d, Math: %d, Physics: %d\n", 
               i + 1, p[i].name, p[i].eng, p[i].math, p[i].phys);
    }

    return 0;
}