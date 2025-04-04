#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/* 日志开关, 关闭打印只把宏需改为0即可 */
#define LOG_SWITCH 1

/* 获取时间戳，使用static inline，方便日志API可以被放到头文件中 */
static inline void log_time()
{
    char buffer[26];
    struct tm *tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    //int millisec = tv.tv_usec / 1000;
    int microseconds =  tv.tv_usec % (1000 * 1000);
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 26, "[%H:%M:%S", tm_info);
    printf("%s.%06d]", buffer, microseconds);
}

/* 格式：时间 模块 文件:行号 @@ 函数 日志信息 */
#define log(fmt, ...) do { \
    if (LOG_SWITCH) { \
        log_time(); \
        printf("[%s:%d] " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#endif
