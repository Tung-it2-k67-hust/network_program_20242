#ifndef STUDENT_H
#define STUDENT_H

typedef struct student {
    char name[20];
    int eng;
    int math;
    int phys;
    double mean;
} STUDENT;

void calculate_mean(STUDENT *s);
char* get_grade(double mean);
void print_students(const STUDENT *data, int size);

#endif