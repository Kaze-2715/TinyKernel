#include "uart.h"

#define UART_BASE 0x10000000L

// NS16550A UART 寄存器定义
#define THR 0 // 偏移 0: THR (Transmit Holding Register)，发送保持寄存器：写入要发送的字符
#define RBR 0 // 接收缓冲寄存器：读取接收到的字符，和 THR 共用地址
#define IER 1 // 中断使能寄存器：控制 UART 中断
#define FCR 2 // FIFO 控制寄存器：控制 UART 内部 FIFO 缓冲区的行为
#define LCR 3 // 线路控制
#define MCR 4 // Modem 控制
#define LSR 5 // 偏移 5: LSR (Line Status Register)，线路状态寄存器：读取 UART 状态

#define LSR_DR 0x01  // 第一位掩码，Data Ready，接收缓冲寄存器有数据可读
#define LSR_THRE 0x20 // 第五位掩码，用来检查是否为 1，THR Empty，发送保持寄存器空，准备好发送下一个字符

#define LCR_DLAB 0x80 // 除数锁存访问位
#define LCR_8N1 0x03  // 8位数据，无校验，1位停止

#define FCR_ENABLE 0x01
#define FCR_CLEAR_RX 0x02
#define FCR_CLEAR_TX 0x04

// 通过宏读写寄存器，volatile 防止编译器优化
// 就是一个写死地址来解引用的操作
#define REG(offset) (*(volatile unsigned char *)(UART_BASE + offset))

void uart_init(void)
{
    // REG(IER) = 0x01; // 开启接收数据可用中断，此时有数据到达时， UART 会向 PLIC 中断控制器发送中断请求
    // 关键：确保 DLAB=0，否则访问的是波特率寄存器
    REG(LCR) = 0; // 清除 DLAB，进入正常模式

    // 现在可以安全地设置 IER
    REG(IER) = 0x01; // 仅使能接收中断

    // 使能并清空 FIFO
    REG(FCR) = FCR_ENABLE | FCR_CLEAR_RX | FCR_CLEAR_TX;

    // 可选：设置 8N1 格式（通常默认就是）
    REG(LCR) = LCR_8N1;

    // 设置 Modem 控制寄存器，使能 OUT2（某些实现需要这个位才能产生中断）
    REG(MCR) = 0x08; // OUT2 = 1，这是中断输出的总开关
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

char uart_getc(void) {
    // 轮询：等待接收器有数据
    while ((REG(LSR) & LSR_DR) == 0)
        ;

    // 读取并返回字符
    return (char)REG(RBR);
}

int uart_has_data(void) {
    // 检查接收缓冲寄存器是否有数据
    return (REG(LSR) & LSR_DR) != 0;
}