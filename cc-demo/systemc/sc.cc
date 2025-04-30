#include <unistd.h>
#include "systemc.h"
#include "log.h"

SC_MODULE(SimpleModule) {
    SC_CTOR(SimpleModule) { // 表示 SC_CONSTRUCTOR 
        SC_THREAD(thread1); // 注册第一个 SC_THREAD 进程
        SC_THREAD(thread2); // 注册第二个 SC_THREAD 进程
    }

    void thread1() {
        for (int i = 0; i < 5; ++i) { // 运行 5 秒后退出
            log("thread1: Current simulation time: %s", sc_time_stamp().to_string().c_str());
            wait(1, SC_SEC); // 等待 1 秒
            if (i == 3) {
                log("begin: unistd sleep 2 seconds");
                sleep(2);
                log("end: unistd sleep 2 seconds");
                //for(;;){usleep(10);}
            }
        }
    }

    void thread2() {
        for (int i = 0; i < 5; ++i) { // 运行 5 秒后退出
            log("thread2: Current simulation time: %s", sc_time_stamp().to_string().c_str());
            wait(1, SC_SEC); // 等待 1 秒
        }
    }
};

int sc_main(int argc, char* argv[]) {
    SimpleModule simple_module("simple_module");
    sc_start(); // 开始仿真
    return 0;
}

/*
1. SystemC默认用的是协程，在OS视角看是其实是单thread，所以遇到OS函数调用带来的OS线程的阻塞，
会阻塞真个协程调度器，即所有的协程。

2. 同样，SystemC是非抢占式的调度，如果一个SC_THREAD陷入了 deadloop，那么所有的SC_THREAD都会
得不到调度了。

运行的打印如下：
        SystemC 3.0.0-Accellera --- Aug  7 2024 23:00:37
        Copyright (c) 1996-2024 by all Contributors,
        ALL RIGHTS RESERVED
[23:45:37.491012][sc.cc:13] thread1: Current simulation time: 0 s
[23:45:37.491059][sc.cc:25] thread2: Current simulation time: 0 s
[23:45:37.491078][sc.cc:13] thread1: Current simulation time: 1 s
[23:45:37.491094][sc.cc:25] thread2: Current simulation time: 1 s
[23:45:37.491099][sc.cc:13] thread1: Current simulation time: 2 s
[23:45:37.491110][sc.cc:25] thread2: Current simulation time: 2 s
[23:45:37.491116][sc.cc:13] thread1: Current simulation time: 3 s
[23:45:37.491120][sc.cc:25] thread2: Current simulation time: 3 s
[23:45:37.491125][sc.cc:16] begin: unistd sleep 2 seconds
[23:45:39.491248][sc.cc:18] end: unistd sleep 2 seconds
[23:45:39.491301][sc.cc:13] thread1: Current simulation time: 4 s
[23:45:39.491341][sc.cc:25] thread2: Current simulation time: 4 s

// GDB 看thread model，可以确认就只有1个thread
Breakpoint 1, SimpleModule::thread1 (this=0x7fffffffdb00) at sc.cc:18
18                      log("end: unistd sleep 2 seconds");
(gdb) bt
#0  SimpleModule::thread1 (this=0x7fffffffdb00) at sc.cc:18
#1  0x00007ffff7eac79f in sc_core::sc_process_b::semantics (this=0x55555556eba0) at ../../../src/sysc/kernel/sc_process.h:633
#2  sc_core::sc_thread_cor_fn (arg=0x55555556eba0) at ../../../src/sysc/kernel/sc_thread_process.cpp:117
#3  0x00007ffff7f68c2b in qt_blocki () at qtmds.s:71
(gdb) info threads
  Id   Target Id             Frame
* 1    process 1460 "sc.out" SimpleModule::thread1 (this=0x7fffffffdb00) at sc.cc:18
||
可以看出，只有1个OS thread，调用站回溯到 qtcmds --- 就是QuickThreads库使用汇编维护的协程栈,
在网上不是那种函数调用栈帧回溯的方法的寻找方式了，所以就找到这里，没有到main程序去
*/