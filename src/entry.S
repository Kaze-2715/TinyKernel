.section .text.entry
.global _start

_start:
    # 设置栈指针（QEMU virt 机器内存通常从 0x80000000 开始，栈顶设在 0x80200000）
    la sp, stack_top

    # 清空 bss 段
    # 将 bss 段起始地址加载到寄存器 t0
    la t0, bss_start
    # 将 bss 段结束地址加载到寄存器 t1
    la t1, bss_end

clear_bss:
    # branch if greater or equal unsigned：如果 t0 >= t1，就跳转到 clear_done
    # 清理结束就跳出循环
    bgeu t0, t1, clear_done
    # 将 zero 寄存器的值写入 t0
    sb zero, 0(t0)
    # 递增 t0
    # 将 t0 的值 + 1，存储到 t0 中
    # addi 目标寄存器，源寄存器，立即数
    addi t0, t0, 1
    # 跳转到开头
    j clear_bss

clear_done:
    call kernel_main

hang:
    j hang

.section .bss
# 内存对齐到 2^12 = 4096B 边界
.align 12
stack_bottom:
    .space 4096
stack_top: