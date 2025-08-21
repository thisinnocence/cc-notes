# AT&T语法的汇编
# GNU汇编器（as）中，默认是AT&T语法

.text
.global entry

entry:
    mov $60, %rax  # syscall number 60 = exit
    mov $1, %rdi   # 传递给exit的参数 1
    syscall
