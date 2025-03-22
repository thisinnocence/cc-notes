/*
 * 这个程序演示了 setjmp 和 longjmp 的使用方法
 * setjmp/longjmp 提供了一种非局部跳转的机制，常用于异常处理
 * 主要用途：
 * 1. 在深层嵌套函数调用中快速返回
 * 2. 实现异常处理机制
 * 3. 处理复杂的错误恢复情况
 */

#include <setjmp.h>
#include <stdio.h>

// 定义跳转缓冲区，用于保存程序上下文
jmp_buf env;

// 模拟一个可能发生错误的函数
void error_function() {
    // longjmp 函数接受两个参数：
    // 1. jmp_buf：由 setjmp 设置的跳转缓冲区
    // 2. value：将作为 setjmp 的返回值（这里是1），不能为0
    longjmp(env, 1); // 发生错误时跳转回 setjmp 的位置
}

int main() {
    // setjmp 第一次调用返回0，用于初始化跳转缓冲区
    // 当 longjmp 调用时，setjmp 会返回 longjmp 指定的值
    if (setjmp(env) == 0) {
        // 正常执行路径
        error_function();
        // 由于 error_function 中的 longjmp，这行代码永远不会执行
        printf("This line should not execute\n");
    } else {
        // longjmp 跳转回来后的执行路径
        printf("Caught error!\n");
    }
    return 0;
}
