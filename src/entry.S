.section .text.entry
.global _start

_start:
    # 设置栈指针（QEMU virt 机器内存通常从 0x80000000 开始，栈顶设在 0x80200000）
    la sp, stack_top

    # 调用 C 函数（先留空，这节课纯汇编输出）
    # call kernel_main
    
    # 直接输出字符 'A' 到 UART
    li t0, 0x10000000       # UART0 基地址 (QEMU virt)
    li t1, 'A'
    sb t1, 0(t0)            # 写入数据寄存器
    
    # 再输出 '\n'
    li t1, '\n'
    sb t1, 0(t0)

# 无限循环，防止跑飞
1:  j 1b

.section .bss
.align 12  # 4KB 对齐
stack_bottom:
    .space 4096  # 4KB 栈空间
stack_top: