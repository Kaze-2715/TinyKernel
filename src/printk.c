#include "printk.h"
#include "uart.h"
// 这个是编译器提供的头文件，不是标准库的一部分
// --nostdlib 指的是不链接 libc 里面的函数
#include <stdarg.h>

static void print_string(const char *s)
{
    uart_puts(s);
}

static void print_int(long num, int base)
{
    char buf[32];
    int i = 0;
    int is_negative = 0;

    if (base == 10 && num < 0)
    {
        is_negative = 1;
        num = -num;
    }

    if (num == 0)
    {
        buf[i++] = '0';
    }
    else
    {
        unsigned long n = num;
        while (n > 0)
        {
            int digit = n % base;
            buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            n /= base;
        }
    }

    if (is_negative)
    {
        buf[i++] = '-';
    }

    while (i > 0)
    {
        uart_putc(buf[--i]);
    }
}

void printk(const char *fmt, ...)
{
    // 声明参数列表变量
    va_list ap;
    // 初始化参数列表，ap 指向第一个可选参数，fmt 是最后一个固定参数
    va_start(ap, fmt);

    while (*fmt)
    {
        if (*fmt == '%' && *(fmt + 1))
        {
            fmt++;
            switch (*fmt)
            {
            case 'c':
            // va_arg(ap, int) 获取下一个参数，类型为 int（char 会被提升为 int）
                uart_putc(va_arg(ap, int));
                break;
            case 's':
                print_string(va_arg(ap, char *));
                break;
            case 'd':
                print_int(va_arg(ap, int), 10);
                break;
            case 'x':
                print_int(va_arg(ap, unsigned int), 16);
                break;
            case '%':
                uart_putc('%');
                break;
            default:
                uart_putc('%');
                uart_putc(*fmt);
                break;
            }
        }
        else
        {
            uart_putc(*fmt);
        }
        fmt++;
    }
    // 清理参数列表
    va_end(ap);
}