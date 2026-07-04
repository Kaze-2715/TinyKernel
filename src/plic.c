#include "plic.h"

#define PLIC_BASE 0x0C000000L

// 寄存器偏移计算
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_ENABLE(context, id) (PLIC_BASE + 0x2000 + (context) * 0x80 + (id) / 32 * 4)
#define PLIC_THRESHOLD(context) (PLIC_BASE + 0x200000 + (context) * 0x1000)
#define PLIC_CLAIM(context) (PLIC_BASE + 0x200004 + (context) * 0x1000)

#define UART_IRQ 10 // QEMU 中 UART 的中断号
#define PLIC_S_CONTEXT 1 // QEMU virt: hart 0 的 S-mode PLIC context

void plic_init(void) {
    // 设置 UART 中断优先级为 1（非零表示有效）
    *(volatile unsigned int *)PLIC_PRIORITY(UART_IRQ) = 1;

    // 启用 hart 0 S-mode context 的 UART 中断
    *(volatile unsigned int *)PLIC_ENABLE(PLIC_S_CONTEXT, UART_IRQ) = (1 << UART_IRQ);

    // 设置全局中断阈值为 0，允许所有优先级的中断
    *(volatile unsigned int *)PLIC_THRESHOLD(PLIC_S_CONTEXT) = 0;
}

int plic_claim(void) {
    // 获取 hart 0 S-mode context 的中断请求
    return *(volatile unsigned int *)PLIC_CLAIM(PLIC_S_CONTEXT);
}

void plic_complete(int irq) {
    // 完成中断处理
    *(volatile unsigned int *)PLIC_CLAIM(PLIC_S_CONTEXT) = irq;
}
