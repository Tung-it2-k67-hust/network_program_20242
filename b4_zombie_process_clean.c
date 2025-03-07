#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // Thêm thư viện này

int main() {
    pid_t pid = fork(); // Tạo tiến trình con

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("I am the child process with PID: %d\n", getpid());
        sleep(5); // Tiến trình con ngủ 5 giây
        printf("Child process is finishing.\n");
        exit(0); // Kết thúc tiến trình con
    } else {
        printf("I am the parent process with PID: %d\n", getpid());
        sleep(30); // Tiến trình cha ngủ 30 giây

        // Thu gom tiến trình zombie
        wait(NULL); // Chờ tiến trình con kết thúc
        printf("Tiến trình cha đã thu gom tiến trình zombie.\n");
    }

    return 0;
}

//wait thu hồi tiến trình con đã kết thúc