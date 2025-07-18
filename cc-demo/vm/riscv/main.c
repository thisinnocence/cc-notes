// add 函数作为一个普通的 C 函数存在。
int add(int a, int b)
{
    return a + b;
}

// _start 是程序的入口点，也是 RISC-V 仿真器开始执行的第一段 C 代码。
void _start()
{
    volatile int a = 1; // 使用 volatile 避免编译器优化
    volatile int b = 2;
    volatile int sum = 0;

    sum = add(a, b); // 调用 add 函数，现在它会被真正执行

    // 裸机环境下，_start 函数通常不会返回。
    // 进入无限循环，让仿真器持续运行。
    while (1) {
        // 在这里可以添加特殊的指令，例如 'ebreak'，
        // 供仿真器检测以表示程序完成或作为断点。
        // asm volatile("ebreak");
    }
}

