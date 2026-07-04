#include "timer.h"

// Sstc: Supervisor-mode Timer Compare extension。
// time 是当前硬件时间计数，stimecmp 是 S-mode 的下一次定时中断比较值。
#define TIMER_INTERVAL 10000000UL

static unsigned long ticks;

static inline unsigned long timer_read_time(void)
{
    unsigned long value;
    asm volatile("csrr %0, time" : "=r"(value));
    return value;
}

static inline void timer_write_stimecmp(unsigned long value)
{
    // stimecmp CSR 编号是 0x14d。使用编号避免工具链不认识 CSR 名称。
    asm volatile("csrw 0x14d, %0" ::"r"(value));
}

static void timer_set_next(void)
{
    timer_write_stimecmp(timer_read_time() + TIMER_INTERVAL);
}

void timer_init(void)
{
    ticks = 0;
    timer_set_next();
}

void timer_handle_interrupt(void)
{
    ticks++;
    timer_set_next();
}

unsigned long timer_ticks(void)
{
    return ticks;
}
