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
    call clear_bss

    # 设置 S-mode 异常向量基址
    la t0, trap_handler
    csrw stvec, t0

    # 设置中断委托
    li t0, 0xffff
    csrw medeleg, t0
    csrw mideleg, t0

    # 允许 S-mode 中断, SIE 位置为 1
    csrsi sstatus, 0x2


    # 配置 mstatus: MPP=01 (S-mode)，MPIE=1
    csrr t0, mstatus
    li t1, ~(0b11 << 11)
    and t0, t0, t1          # 清除MPP
    li t1, (0b01 << 11) | (1 << 7)
    or t0, t0, t1           # 设置MPP=01, MPIE=1
    csrw mstatus, t0

    # 设置返回地址为 kernel_main，执行 mret 进入 S-mode
    # 配置PMP：允许S-mode访问全部内存（0x00000000-0xFFFFFFFF）
    # PMPADDR0 = 0x3FFFFFFFF（覆盖4GB，右移2位后）
    # PMPCFG0 = 0x0F（L=0, A=TOR, X=1, W=1, R=1）
    
    li t0, 0x3FFFFFFFF
    csrw pmpaddr0, t0       # 范围上限
    
    li t0, 0x0F             # TOR模式，RWX权限，不锁定
    csrw pmpcfg0, t0        # 配置生效
    
    la t0, kernel_main
    csrw mepc, t0;
    mret

    # 死循环保险
1:  j   1b

trap_handler:
    .align 4
    # 保存调用者保存的寄存器（C函数可能修改它们）
    addi sp, sp, -256       # 分配256字节栈帧
    sd ra, 0(sp)            # 保存返回地址
    sd t0, 8(sp)            # 保存临时寄存器
    sd t1, 16(sp)
    sd t2, 24(sp)
    sd a0, 32(sp)           # 保存参数/返回值寄存器
    sd a1, 40(sp)
    sd a2, 48(sp)
    sd a3, 56(sp)
    sd a4, 64(sp)
    sd a5, 72(sp)
    sd a6, 80(sp)
    sd a7, 88(sp)
    # t3-t6 也保存（如果C编译器用到）
    sd t3, 96(sp)
    sd t4, 104(sp)
    sd t5, 112(sp)
    sd t6, 120(sp)

    # 调用 C 处理函数
    call trap_handler_c

    # 恢复寄存器
    ld ra, 0(sp)
    ld t0, 8(sp)
    ld t1, 16(sp)
    ld t2, 24(sp)
    ld a0, 32(sp)
    ld a1, 40(sp)
    ld a2, 48(sp)
    ld a3, 56(sp)
    ld a4, 64(sp)
    ld a5, 72(sp)
    ld a6, 80(sp)
    ld a7, 88(sp)
    ld t3, 96(sp)
    ld t4, 104(sp)
    ld t5, 112(sp)
    ld t6, 120(sp)
    addi sp, sp, 256

    # 从异常返回
    sret                    # 使用 sepc 返回（注意：不是 mret！）

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
    ret

.section .bss
# 内存对齐到 2^12 = 4096B 边界
.align 12
stack_bottom:
    .space 4096
stack_top: