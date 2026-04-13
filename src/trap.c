#include "uart.h"
#include "printk.h"

// 读取 S-mode CSR 的内联汇编
#define read_csr(reg) ({                          \
    unsigned long __tmp;                          \
    asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                        \
})

void trap_handler_c(void)
{
    unsigned long scause = read_csr(scause);
    unsigned long sepc = read_csr(sepc);
    unsigned long stval = read_csr(stval);

    printk("\n\n!!! TRAP OCCURRED !!!\n");
    printk("scause: 0x%x\n", scause);
    printk("sepc:   0x%x\n", sepc);
    printk("stval:  0x%x\n", stval);

    // 解析 scause 原因
    if (scause & (1UL << 63))
    {
        printk("Type: Interrupt\n");
    }
    else
    {
        printk("Type: Exception\n");
        switch (scause & 0x7FFFFFFFFFFFFFFFUL)
        {
        case 0:
            printk("Cause: Instruction address misaligned\n");
            break;
        case 1:
            printk("Cause: Instruction access fault\n");
            break;
        case 2:
            printk("Cause: Illegal instruction\n");
            break;
        case 3:
            printk("Cause: Breakpoint\n");
            break;
        case 4:
            printk("Cause: Load address misaligned\n");
            break;
        case 5:
            printk("Cause: Load access fault\n");
            break;
        case 6:
            printk("Cause: Store/AMO address misaligned\n");
            break;
        case 7:
            printk("Cause: Store/AMO access fault\n");
            break;
        case 12:
            printk("Cause: Instruction page fault\n");
            break;
        case 13:
            printk("Cause: Load page fault\n");
            break;
        case 15:
            printk("Cause: Store/AMO page fault\n");
            break;
        default:
            printk("Cause: Unknown (%d)\n", scause);
            break;
        }
    }
    // 新增：判断是否为可跳过异常
    unsigned long cause_code = scause & 0x7FFFFFFFFFFFFFFFUL;

    if (cause_code == 2)
    { // 非法指令
        printk("Skipping illegal instruction...\n");
        sepc += 4; // 跳过4字节（RV64指令长度）
        // 写回 sepc
        asm volatile("csrw sepc, %0" ::"r"(sepc));
        // 返回后继续执行
        return;
    }

    // 其他异常：死循环
    printk("Fatal error, halting.\n");

    // 死循环停机
    while (1)
    {
        asm volatile("wfi"); // 等待中断，降低功耗
    }
}