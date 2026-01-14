OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
  /* 模拟 qemu virt 的 DDR 基地址；解释器会把 ELF 的 p_vaddr 直接映射到此内存。 */
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64K
}

PHDRS {
  /* 只映射代码与数据，ELF/PHDR 不需要映射到内存。 */
  text PT_LOAD FLAGS(0x5); /* R + X */
  data PT_LOAD FLAGS(0x6); /* R + W */
}

SECTIONS {
  /* 段起始地址固定在 DDR 基址。 */
  . = ORIGIN(RAM);

  /* 启动入口放在最前，确保 _start 在可执行段起始处。 */
  .text : {
    /* 启动代码单独放在 .text.init，避免被链接器优化掉。 */
    *(.text.init)
    /* 其余代码放在常规 .text 段。 */
    *(.text .text.*)
  } > RAM :text

  /* 只读数据：字符串常量等（本 demo 里几乎不用）。 */
  .rodata : {
    *(.rodata .rodata.*)
  } > RAM :text

  /* 已初始化的可写数据。 */
  .data : {
    *(.data .data.*)
  } > RAM :data

  /* 未初始化的全局/静态变量，运行时会清零。 */
  .bss : {
    *(.bss .bss.*)
    *(COMMON)
  } > RAM :data

  /DISCARD/ : {
    *(.note.gnu.build-id)
    *(.note .note.*)
  }

  /* 栈顶放在 RAM 末尾，start.S 会把 sp 指到这里。 */
  . = ALIGN(16);
  _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
