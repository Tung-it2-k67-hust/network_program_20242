#include <iostream>
using namespace std;

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

STUDENT *p;

int main() {
    p = data;
    cout << "Name\tEng\tMath\tPhys" << endl;
    for (int i = 0; i < 4; i++) {
        cout << p->name << "\t" << p->eng << "\t" << p->math << "\t" << p->phys << endl;
        p++;
    }
    return 0;
}