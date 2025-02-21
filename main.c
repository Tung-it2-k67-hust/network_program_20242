#include <stdio.h>
#include "student.h"

static STUDENT data[] = {
    {"Tuan", 82, 72, 58, 0.0},
    {"Nam", 77, 82, 79, 0.0},
    {"Khanh", 52, 62, 39, 0.0},
    {"Phuong", 61, 82, 88, 0.0}
};

int main() {
    int size = sizeof(data) / sizeof(data[0]);
    STUDENT *p = data; // Pointer p points to the data array

    // Calculate the mean for each student
    for (int i = 0; i < size; i++) {
        calculate_mean(&p[i]);
    }

    // Print the results
    print_students(p, size);

    return 0;
}