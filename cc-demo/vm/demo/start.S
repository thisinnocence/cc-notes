.section .text.init
.globl _start
_start:
  # 设置栈指针到链接脚本里定义的 _stack_top（PC 相对寻址）
  la sp, _stack_top
  # 跳转到 C 的 main
  call main
  # main 返回后触发非法指令，解释器会打印并退出
  .word 0
