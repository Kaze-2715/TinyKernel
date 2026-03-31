#include "uart.h"

#define UART_BASE 0x10000000L

// NS16550A UART 寄存器定义
// 偏移 0: THR (Transmit Holding Register) 
// 发送保持寄存器：写入要发送的字符
#define THR 0
// 偏移 5: LSR (Line Status Register) 
// 线路状态寄存器：读取 UART 状态
#define LSR 5

// 第五位掩码，用来检查是否为 1，THR Empty，发送保持寄存器空，准备好发送下一个字符
// 这个寄存器的每一位标识不同的状态
#define LSR_THRE 0x20

// 通过宏读写寄存器，volatile 防止编译器优化
// 就是一个写死地址来解引用的操作
#define REG(offset) (*(volatile unsigned char *)(UART_BASE + offset))

void uart_init(void)
{
    // QEMU 的 UART 不需要初始化波特率
    // 实际硬件这里要配置 divisor 等
}

void uart_putc(char c)
{
    // 轮询：等待发送器就绪
    while ((REG(LSR) & LSR_THRE) == 0)
        ;

    // 写入字符
    REG(THR) = c;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}