# CFLAGS: C 编译选项
#   -nostdlib: 不链接标准系统启动文件和库。对于裸机程序是必不可少的。
#   -nostartfiles: 不链接标准系统启动文件（如 crt0.o）。我们自己定义程序入口点 _start。
#   -march=rv64gc: 指定 RISC-V 架构。rv64gc 包括整数 (I)、乘除 (M)、原子 (A)、
#                   压缩指令 (C) 和浮点 (F/D) 扩展。
#   -mabi=lp64d: 指定 RISC-V ABI (Application Binary Interface)。lp64d 适用于 64 位系统，
#                支持双精度浮点数。
CFLAGS = -nostdlib -nostartfiles -march=rv64gc -mabi=lp64d

# LDFLAGS: 链接器选项
#   -T link.ld: 使用我们自定义的链接脚本 'link.ld' 来定义内存布局和程序段。
#   -static: 强制进行静态链接。对于裸机程序至关重要，它会移除对共享库和动态链接相关段
#            （如 .interp, .dynsym, .dynamic 等）的依赖。
#   -Wl,--gc-sections: 将 '--gc-sections' 选项传递给链接器 ld。这会指示链接器移除
#                      未被使用的代码和数据段（即进行垃圾回收），从而进一步减小二进制文件大小。
#   -ffreestanding: 告诉编译器，它正在为一个“独立”环境（没有操作系统，没有标准 C 库）编译代码。
#                   这有助于避免对完整标准库的默认假设。
LDFLAGS = -T link.ld -static -Wl,--gc-sections -ffreestanding

rv64.bin: main.c
	riscv64-linux-gnu-gcc $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.bin