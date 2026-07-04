#include "uart.h"
#include "printk.h"
#include "plic.h"
#include "timer.h"

void kernel_main(void)
{
    uart_init();
    plic_init();
    timer_init();

    printk("Hello from printk!\n");
    printk("Kernel running in S-mode with PLIC enabled\n");

    while (1)
    {
        // 内联汇编，等待中断
        // wait for interrupt
        asm volatile("wfi");
    }
}
