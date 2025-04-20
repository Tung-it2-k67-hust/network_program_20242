#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork(); // Tạo tiến trình con

    if (pid < 0) {
        // Xử lý lỗi khi fork thất bại
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Đây là tiến trình con
        printf("Con voi PID: %d\n", getpid());
        sleep(5); // Tiến trình con ngủ 5 giây
        printf("con ket thuc.\n");
        exit(0); // Kết thúc tiến trình con
    } else {
        // Đây là tiến trình cha
        printf("Cha voi PID: %d\n", getpid());
        sleep(30); // Tiến trình cha ngủ 30 giây
        printf("cha da xong.\n");
    }

    return 0;
}

/*
lệnh $ ps aux | grep Z hoặc mở lệnh top trong khi tiến trình cha 
đang ngủ (30 giây) và tiến trình con đã hoàn thành, bạn sẽ thấy 
tiến trình con xuất hiện trong danh sách với trạng thái "Z" 
(để chỉ ra rằng nó là một tiến trình zombie).


Tiến trình nào đang ở trạng thái Z?: Tiến trình con mà bạn đã tạo ra bằng fork() 
và đã hoàn thành công việc của nó. Nó tồn tại dưới dạng zombie 
cho đến khi được thu hồi bởi tiến trình cha.


Từ khóa <defunct> nghĩa là gì?: Từ khóa <defunct> là một cách 
khác để chỉ ra rằng tiến trình đó là một tiến trình zombie. 
Nó cho biết rằng tiến trình đã hoàn thành nhưng chưa được thu hồi,
 và do đó vẫn còn tồn tại trong bảng tiến trình.

 */