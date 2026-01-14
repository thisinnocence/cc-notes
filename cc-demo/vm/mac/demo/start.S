.section .text.init
.globl _start
_start:
  # 设置栈指针到链接脚本里定义的 _stack_top
  la sp, _stack_top
  # 跳转到 C 的 main
  call main
1:
  # main 返回后进入死循环，方便解释器以非法指令/越界判定退出
  j 1b
