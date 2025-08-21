ENTRY(entry)  /* 指定入口点为 entry */

SECTIONS {
    . = 0x400000;  /* 可执行文件加载地址（可选） */
    .text : { *(.text) }
    /DISCARD/ : { *(*) }  /* 丢弃其他未使用的段 */
}
