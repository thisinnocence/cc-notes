// F_GETPIPE_SZ need this macro
 #define _GNU_SOURCE

#include <glib.h>
#include <glib-unix.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
    // fd[0] read
    // fd[1] write
    gint fds[2];

    GError *error = NULL;

    // 创建管道, FD_CLOEXEC 表示管道不会被子进程继承
    if (!g_unix_open_pipe(fds, FD_CLOEXEC, &error)) {
        // 如果失败，打印错误信息
        g_printerr("Failed to create pipe: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    int pipe_size = fcntl(fds[0], F_GETPIPE_SZ);
    printf("pipe size is %d\n", pipe_size);

    // 管道创建成功，写入一些数据
    //   管道可以帮助避免竞争条件、同步问题，
    //   因为管道在操作系统层面实现了缓冲区管理和线程安全。
    //  note： 默认当wirte超过了pipe的大小，会阻塞，直到read来读取后才会继续执行
    //         当然也可以设置成非阻塞
    const char *message = "Hello, pipe!";
    write(fds[1], message, strlen(message));
    printf("write pipe msg: %s\n", message);

    // 读取数据
    char buffer[128];
    ssize_t bytes_read = read(fds[0], buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // 添加字符串终止符
        printf("read pipe msg: %s\n", buffer);
    }

    // 关闭管道
    close(fds[0]);
    close(fds[1]);

    return 0;
}

