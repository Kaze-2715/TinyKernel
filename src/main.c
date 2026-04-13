#include "uart.h"
#include "printk.h"

void kernel_main(void)
{
    uart_init();

    // uart_puts("Hello from C!\n");
    // uart_putc('O');
    // uart_putc('K');
    // uart_putc('\n');

    printk("Hello from printk!\n");
    printk("Char: %c\n", 'X');
    printk("String: %s\n", "tinykernel");
    printk("Decimal: %d, Negative: %d\n", 42, -123);
    printk("Hex: 0x%x\n", 0xABCD);
    printk("Mixed: %c %s %d 0x%x\n", 'A', "test", 255, 255);

    printk("Kernel running in S-mode\n");

    // 故意触发非法指令异常
    printk("Triggering illegal instruction...\n");

    // 内联汇编执行一条非法指令
    // 0x00000000 是非法编码
    asm volatile(".word 0x00000000");

    printk("After illegal instruction (skipped!)\n"); // 应该能执行到

    while (1)
    {
        // 内联汇编，等待中断
        // wait for interrupt
        asm volatile("wfi");
    }
}