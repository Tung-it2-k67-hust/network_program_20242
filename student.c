#include <stdio.h>
#include <stdlib.h>
#include "student.h"

void calculate_mean(STUDENT *s) {
    s->mean = (s->eng + s->math + s->phys) / 3.0;
}

char* get_grade(double mean) {
    if (mean >= 90) return "S";
    else if (mean >= 80) return "A";
    else if (mean >= 70) return "B";
    else if (mean >= 60) return "C";
    else return "D";
}

int compare_students(const void *a, const void *b) {
    STUDENT *studentA = (STUDENT *)a;
    STUDENT *studentB = (STUDENT *)b;
    if (studentA->mean < studentB->mean) return 1;
    if (studentA->mean > studentB->mean) return -1;
    return 0;
}

void print_students(STUDENT *data, int size) {
    // Sort the students in descending order based on their mean scores
    qsort(data, size, sizeof(STUDENT), compare_students);

    printf("%-10s %-5s %-5s %-5s %-10s %-5s\n", "Name", "Eng", "Math", "Phys", "Mean", "Grade");
    for (int i = 0; i < size; i++) {
        printf("%-10s %-5d %-5d %-5d %-10.2f %-5s\n", 
               data[i].name, data[i].eng, data[i].math, data[i].phys, data[i].mean, get_grade(data[i].mean));
    }
}