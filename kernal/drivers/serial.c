/**
 * @file serial.c
 * @brief 16550 UART driver for PinguinOS (COM1, 115200 8N1).
 */

#include "../include/serial.h"
#include "../include/cpu.h"
#include "../include/klib.h"

static bool serial_ready = false;

bool serial_init(void)
{
    /* Disable all interrupts */
    outb(SERIAL_COM1 + UART_IER, 0x00);

    /* Enable DLAB (divisor latch access bit) in LCR */
    outb(SERIAL_COM1 + UART_LCR, 0x80);

    /* Set divisor to 1 → 115200 baud (base clock = 115200 * divisor) */
    outb(SERIAL_COM1 + UART_DATA, 0x01);   /* Low  byte of divisor */
    outb(SERIAL_COM1 + UART_IER,  0x00);   /* High byte of divisor */

    /* 8 bits, no parity, one stop bit (clear DLAB) */
    outb(SERIAL_COM1 + UART_LCR, 0x03);

    /* Enable FIFO, clear queues, 14-byte threshold */
    outb(SERIAL_COM1 + UART_FCR, 0xC7);

    /* RTS + DTR (ready to send) */
    outb(SERIAL_COM1 + UART_MCR, 0x0B);

    /* Loopback test: write 0xAE and verify readback */
    outb(SERIAL_COM1 + UART_MCR, 0x1E);   /* Enable loopback */
    outb(SERIAL_COM1 + UART_DATA, 0xAE);  /* Send test byte  */

    if (inb(SERIAL_COM1 + UART_DATA) != 0xAE) {
        serial_ready = false;
        return false;
    }

    /* Disable loopback, set normal operation mode */
    outb(SERIAL_COM1 + UART_MCR, 0x0F);
    serial_ready = true;
    return true;
}

void serial_putc(char c)
{
    if (!serial_ready) return;
    /* Wait until transmit holding register is empty */
    while (!(inb(SERIAL_COM1 + UART_LSR) & UART_LSR_THRE))
        pause();
    outb(SERIAL_COM1 + UART_DATA, (uint8_t)c);
}

void serial_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') serial_putc('\r');
        serial_putc(*str++);
    }
}

void serial_printf(const char *fmt, ...)
{
    char buf[512];
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    __builtin_va_end(ap);
    serial_puts(buf);
}

bool serial_has_data(void)
{
    return serial_ready && (inb(SERIAL_COM1 + UART_LSR) & UART_LSR_DR);
}

char serial_getc(void)
{
    if (!serial_ready) return 0;
    while (!serial_has_data()) pause();
    return (char)inb(SERIAL_COM1 + UART_DATA);
}

/* ── Kernel log ──────────────────────────────────────────────────── */
static const char *level_str[] = { "DBG", "INF", "WRN", "ERR", "!!!" };

void klog(log_level_t level, const char *fmt, ...)
{
    char buf[512];
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    __builtin_va_end(ap);

    const char *lvl = (level <= LOG_PANIC) ? level_str[level] : "???";
    serial_printf("[%s] %s", lvl, buf);
}
