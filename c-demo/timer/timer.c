#define _GNU_SOURCE  // 这样才能用 REG_RIP
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"

void timer_handler(int sig, siginfo_t *si, void *uc) {
    // @file: bits/types/siginfo_t.h
    // #define si_value	_sifields._rt.si_sigval
    /* (gdb) pt si
    type = struct {
        int si_signo;
        int si_errno;
        int si_code;
        int __pad0;
        union {
            int _pad[28];
            struct {...} _kill;
            struct {...} _timer;
            struct {...} _rt;
            struct {...} _sigchld;
            struct {...} _sigfault;
            struct {...} _sigpoll;
            struct {...} _sigsys;
        } _sifields;
    } */
    int *data = (int *)si->si_value.sival_ptr;
    log("timeout proc, sig %d, data %d, uc %p", sig, *data, uc);
    ucontext_t *context = (ucontext_t *)uc;
    log("timeout proc int thread 0x%lx", pthread_self());
    //     (gdb) pt context
    //      type = struct ucontext_t {
    //          unsigned long uc_flags;
    //          struct ucontext_t *uc_link;
    //          stack_t uc_stack;
    //          mcontext_t uc_mcontext;
    //          sigset_t uc_sigmask;
    //          struct _libc_fpstate __fpregs_mem;
    //          unsigned long long __ssp[4];
    //      } *
    //(gdb) pt context->uc_mcontext.gregs
    //      type = long long [23]
    log("Context RIP: %lx", (unsigned long)context->uc_mcontext.gregs[REG_RIP]);
}

int main() {
    int data = 21;
    struct sigevent sev;
    timer_t timerid;
    struct itimerspec its;
    struct sigaction sa;
    log("main thread 0x%lx", pthread_self());


    // 设置信号处理函数
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    // https://man7.org/linux/man-pages/man2/sigaction.2.html
    sigaction(SIGRTMIN, &sa, NULL);
    /* backtrace, 仍然复用的是当前程序自己thread stack
    #0  timer_handler (sig=32767, si=0x7fff00000009, uc=0x555555556020) at timer.c:7
    #1  <signal handler called>
    #2  0x00007ffff7e9de47 in ?? ()
    #3  0x00005555555554d3 in main () at timer.c:94 --> pause() */

    // 创建定时器
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    sev.sigev_value.sival_ptr = &data; // 将数据指针传递给sigev_value
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    // 启动定时器，设置1秒后触发，之后每隔1秒触发一次
    // struct itimerspec {
    //     struct timespec it_interval;  /* Interval for periodic timer */
    //     struct timespec it_value;     /* Initial expiration */
    // };

    //  定时器初始第一次的到期时间
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;

    // 到期后，周期触发的间隔时间，如果是0，就是单次定时器了
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    timer_settime(timerid, 0, &its, NULL);

    // 主线程等待信号处理
    log("run before pause");
    pause();
    log("run after pause");

    return 0;
}

