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

    while (1)
    {
        // 内联汇编，等待中断
        // wait for interrupt
        asm volatile("wfi");
    }
}