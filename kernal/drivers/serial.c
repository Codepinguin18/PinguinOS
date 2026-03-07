/**
 * @file serial.c
 * @brief 16550 UART-Treiber für PinguinOS (COM1, 115200 8N1).
 */

#include "../include/serial.h"
#include "../include/cpu.h"
#include "../include/klib.h"

static bool serial_ready = false;

bool serial_init(void)
{
    /* Alle Interrupts deaktivieren */
    outb(SERIAL_COM1 + UART_IER, 0x00);

    /* DLAB (Divisor Latch Access Bit) im LCR aktivieren */
    outb(SERIAL_COM1 + UART_LCR, 0x80);

    /* Divisor auf 1 setzen → 115200 Baud (Basistakt = 115200 * Divisor) */
    outb(SERIAL_COM1 + UART_DATA, 0x01);   /* Low-Byte des Divisors */
    outb(SERIAL_COM1 + UART_IER,  0x00);   /* High-Byte des Divisors */

    /* 8 Bits, keine Parität, ein Stoppbit (DLAB löschen) */
    outb(SERIAL_COM1 + UART_LCR, 0x03);

    /* FIFO aktivieren, Queues löschen, 14-Byte Schwellenwert */
    outb(SERIAL_COM1 + UART_FCR, 0xC7);

    /* RTS + DTR (Ready To Send) */
    outb(SERIAL_COM1 + UART_MCR, 0x0B);

    /* Loopback-Test: 0xAE schreiben und verifizieren */
    outb(SERIAL_COM1 + UART_MCR, 0x1E);   /* Loopback aktivieren */
    outb(SERIAL_COM1 + UART_DATA, 0xAE);  /* Test-Byte senden */

    if (inb(SERIAL_COM1 + UART_DATA) != 0xAE) {
        serial_ready = false;
        return false;
    }

    /* Loopback deaktivieren, normalen Betriebsmodus setzen */
    outb(SERIAL_COM1 + UART_MCR, 0x0F);
    serial_ready = true;
    return true;
}

void serial_putc(char c)
{
    if (!serial_ready) return;
    /* Warten bis Sende-Holding-Register leer ist */
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

/* ── Kernel-Log ──────────────────────────────────────────────────── */
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
