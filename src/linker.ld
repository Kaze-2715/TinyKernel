OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    /* "." 是位置计数器，相当于一个变量，存储的是当前地址 */
    . = 0x80200000;  /* QEMU -kernel 加载地址 */

    .text : {
        *(.text.entry)
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        /* 这是一个链接器符号，不是变量，它用来记录地址信息 */
        bss_start = .;
        *(.bss .bss.*)
        /* 特殊的链接机制，用来处理未初始化的全局变量 */
        *(COMMON)
        bss_end = .;
    }
}