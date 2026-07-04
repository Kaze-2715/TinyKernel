#include "uart.h"
#include "printk.h"
#include "plic.h"

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
    unsigned long cause_code = scause & 0x7FFFFFFFFFFFFFFFUL; // 取低63位作为中断号

    printk("\n\n[Trap]\n");
    printk("scause: 0x%x\n", scause);
    printk("sepc:   0x%x\n", sepc);
    printk("stval:  0x%x\n", stval);

    // 解析 scause 原因
    if (scause & (1UL << 63))
    {
        printk("Type: Interrupt\n");

        if (cause_code == 9)
        {
            int irq = plic_claim();

            if (irq == 10)
            {
                char c = uart_getc();
                printk("[UART IRQ] received: %c (0x%x)\n", c, c);
            }
            else if (irq != 0)
            {
                printk("[External IRQ] unknown irq: %d\n", irq);
            }

            plic_complete(irq);
            return; // 处理完中断后返回继续执行
        }
        // 其他中断（时钟、软件）暂不处理
        printk("[Interrupt] code: %d, not handled\n", cause_code);
        return;
    }

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
