#include <systemc.h>
#include <pthread.h>
#include <unistd.h>
#include <async_event.h> // see Makefile: it's from $(SYSTEMC_HOME)/examples/sysc/async_suspend

#include "log.h"

async_event async_event;

SC_MODULE(AsyncExample) {
    SC_CTOR(AsyncExample) {
        SC_THREAD(process_event);
    }

    void process_event() {
        int cnt = 0;
        for(;;) {
            wait(async_event);  // 等待异步事件触发
            log("Async event caught at %f cnt[%d]", sc_time_stamp().to_seconds(), cnt);
            if (cnt++ > 1) {
                exit(0);
            }
        }
    }
};

void* os_thread(void* arg) {
    while (true) {
        sleep(1);  // 模拟一些外部事件
        log("OS thread: Triggering async event at %f", sc_time_stamp().to_seconds());
        async_event.notify();  // 异步通知 SystemC 事件
    }
    return nullptr;
}

int sc_main(int argc, char* argv[]) {
    AsyncExample example("AsyncExample");

    pthread_t tid;
    pthread_create(&tid, nullptr, os_thread, nullptr);

    sc_start();  // 启动仿真
    pthread_join(tid, nullptr);
    return 0;
}

/*
OS线程可以和SC_THREAD交替运行，对于 event 只有SC调度的协程中触发使用，跨OS的thread需要使用这个实现。
also see:  https://github.com/Xilinx/libsystemctlm-soc/blob/master/utils/async_event.h

[New Thread 0x7ffff7476700 (LWP 7374)]
[11:10:00.401090][async_event.cc:30] OS thread: Triggering async event at 0.000000
[11:10:00.401274][async_event.cc:19] Async event caught at 0.000000 cnt[0]

Thread 1 "async_event.out" hit Breakpoint 1, AsyncExample::process_event (this=0x7fffffffda60) at async_event.cc:20
warning: Source file is more recent than executable.

(gdb) info threads
  Id   Target Id                                          Frame
* 1    Thread 0x7ffff787b740 (LWP 7370) "async_event.out" AsyncExample::process_event (this=0x7fffffffda60) at async_event.cc:20
  2    Thread 0x7ffff7476700 (LWP 7374) "async_event.out" 0x00007ffff7aac23f in __GI___clock_nanosleep () at ../sysdeps/unix/sysv/linux/clock_nanosleep.c:78

(gdb) t a a bt
#0  0x00007ffff7aac23f in __GI___clock_nanosleep () at ../sysdeps/unix/sysv/linux/clock_nanosleep.c:78
#1  0x00007ffff7ab1ec7 in __GI___nanosleep (requested_time=requested_time@entry=0x7ffff7475e90, remaining=remaining@entry=0x7ffff7475e90) at nanosleep.c:27
#2  0x00007ffff7ab1dfe in __sleep (seconds=0) at ../sysdeps/posix/sleep.c:55
#3  0x00005555555575d0 in os_thread (arg=0x0) at async_event.cc:29
#4  0x00007ffff7dc6609 in start_thread (arg=<optimized out>) at pthread_create.c:477
#5  0x00007ffff7aee133 in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95

Thread 1 (Thread 0x7ffff787b740 (LWP 7370)):
#0  AsyncExample::process_event (this=0x7fffffffda60) at async_event.cc:20
#1  0x00007ffff7eac79f in sc_core::sc_process_b::semantics (this=0x5555555715b0) at ../../../src/sysc/kernel/sc_process.h:633
#2  sc_core::sc_thread_cor_fn (arg=0x5555555715b0) at ../../../src/sysc/kernel/sc_thread_process.cpp:117
#3  0x00007ffff7f68c2b in qt_blocki () at qtmds.s:71
*/