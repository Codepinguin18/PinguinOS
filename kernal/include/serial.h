/**
 * @file serial.h
 * @brief 16550 UART serial port driver for PinguinOS.
 *
 * Provides text output to COM1 for QEMU's -serial stdio debug console.
 * The serial port is also used for the kernel log (klog_*) functions.
 *
 * Usage with QEMU:
 *   qemu-system-x86_64 -cdrom kernel.iso -m 512M -serial stdio
 */
#ifndef _SERIAL_H
#define _SERIAL_H

#include "types.h"

/* ── Port base addresses ─────────────────────────────────────────── */
#define SERIAL_COM1  0x3F8
#define SERIAL_COM2  0x2F8
#define SERIAL_COM3  0x3E8
#define SERIAL_COM4  0x2E8

/* ── 16550 register offsets ──────────────────────────────────────── */
#define UART_DATA    0   /* Data register (read = RX, write = TX)     */
#define UART_IER     1   /* Interrupt enable register                  */
#define UART_IIR     2   /* Interrupt identification register (read)   */
#define UART_FCR     2   /* FIFO control register (write)              */
#define UART_LCR     3   /* Line control register                      */
#define UART_MCR     4   /* Modem control register                     */
#define UART_LSR     5   /* Line status register                       */
#define UART_MSR     6   /* Modem status register                      */
#define UART_SCRATCH 7

/* Line Status Register bits */
#define UART_LSR_DR    (1 << 0)   /* Data ready          */
#define UART_LSR_THRE  (1 << 5)   /* TX holding reg empty */

/* ── Log levels ──────────────────────────────────────────────────── */
typedef enum log_level {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3,
    LOG_PANIC = 4,
} log_level_t;

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Initialise COM1 at 115200 baud, 8N1.
 * @return true if UART detected and configured, false otherwise.
 */
bool serial_init(void);

/** Write a single byte (blocks until TX buffer is ready). */
void serial_putc(char c);

/** Write a null-terminated string. */
void serial_puts(const char *str);

/**
 * @brief Formatted output to serial (same specifiers as vga_printf).
 */
void serial_printf(const char *fmt, ...);

/** Read a byte from COM1 (blocking). */
char serial_getc(void);

/** @return true if a character is waiting in the RX buffer. */
bool serial_has_data(void);

/* ── Kernel log macros ───────────────────────────────────────────── */
void klog(log_level_t level, const char *fmt, ...);

#define KDBG(fmt, ...)   klog(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define KINFO(fmt, ...)  klog(LOG_INFO,  fmt, ##__VA_ARGS__)
#define KWARN(fmt, ...)  klog(LOG_WARN,  fmt, ##__VA_ARGS__)
#define KERR(fmt, ...)   klog(LOG_ERROR, fmt, ##__VA_ARGS__)

#endif /* _SERIAL_H */
