#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include "util/log.h"

struct timeval timeout = {1, 0}; // 1 seconds timeout

/* 回调函数签名
@evutil_socket_t fd
fd 是文件描述符，表示事件关联的文件描述符或套接字。它的类型是 evutil_socket_t。
  这是一个跨平台的类型定义，通常在 Unix 系统上为 int，在 Windows 上为 SOCKET。
  对于定时器事件或信号事件，这个参数通常没有实际意义，可以忽略。

@short event，可能是一个或者多个时间组合，类似 bitmap，所以需要按位与来确定时间类型
event 是事件类型的标志。它指示了哪个事件触发了回调函数。
libevent 中定义了多种事件类型，包括：
    EV_READ：文件描述符上有数据可读。
    EV_WRITE：文件描述符上可以写数据。
    EV_TIMEOUT：定时器超时。
    EV_SIGNAL：信号事件。
对于定时器事件，这个参数通常是 EV_TIMEOUT。
*/
void on_timeout(evutil_socket_t fd, short event, void *arg) {
    log("Timer expired, fd %d event %d data %d", fd, event, *(int*)arg);
    
}

int main() {
    struct event_base *base;
    struct event *timeout_event;

    // 创建新的事件基础结构
    base = event_base_new();
    if (!base) { log("Could not initialize libevent!"); return 1; }

    // 创建定时器事件
    int data = 21;
    timeout_event = event_new(base, -1, EV_TIMEOUT, on_timeout, &data);
    if (!timeout_event) { log("Could not create event!"); return 1; }

    // 添加定时器事件到事件循环中
    event_add(timeout_event, &timeout);

    // 开始事件循环
    event_base_dispatch(base);

    // 清理资源
    event_free(timeout_event);
    event_base_free(base);
    return 0;
}

/* gdb to check thread model
Breakpoint 1, on_timeout (fd=-1, event=1, arg=0x7fffffffdccc) at libevent.c:22
22           log("Timer expired, fd %d event %d data %d", fd, event, *(int*)arg);
(gdb) info threads 
  Id   Target Id                                    Frame 
* 1    Thread 0x7ffff7d48740 (LWP 7939) "event.out" on_timeout (fd=-1, event=1, arg=0x7fffffffdccc) at libevent.c:22
(gdb) bt
#0  on_timeout (fd=-1, event=1, arg=0x7fffffffdccc) at libevent.c:22
#1  0x00007ffff7f80fde in ?? () from /lib/x86_64-linux-gnu/libevent-2.1.so.7
#2  0x00007ffff7f8187f in event_base_loop () from /lib/x86_64-linux-gnu/libevent-2.1.so.7
#3  0x000055555555548e in main () at libevent.c:43
(gdb) c
Continuing.
[00:08:16.680949][libevent.c:8]Timer expired, fd -1 event 1 data 21
*/
