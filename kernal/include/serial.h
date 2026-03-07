/**
 * @file serial.h
 * @brief 16550 UART Treiber für die serielle Schnittstelle in PinguinOS.
 *
 * Bietet Textausgabe über COM1 für QEMUs -serial stdio Debug-Konsole.
 * Die serielle Schnittstelle wird auch für die Kernel-Log-Funktionen (klog_*) genutzt.
 *
 * Verwendung mit QEMU:
 *   qemu-system-x86_64 -cdrom kernel.iso -m 512M -serial stdio
 */
#ifndef _SERIAL_H
#define _SERIAL_H

#include "types.h"

/* ── Port-Basisadressen ──────────────────────────────────────────── */
#define SERIAL_COM1  0x3F8
#define SERIAL_COM2  0x2F8
#define SERIAL_COM3  0x3E8
#define SERIAL_COM4  0x2E8

/* ── 16550 Register-Offsets ──────────────────────────────────────── */
#define UART_DATA    0   /* Daten-Register (Lesen = RX, Schreiben = TX) */
#define UART_IER     1   /* Interrupt-Enable-Register                  */
#define UART_IIR     2   /* Interrupt-Identifikationsregister (Lesen)   */
#define UART_FCR     2   /* FIFO-Control-Register (Schreiben)           */
#define UART_LCR     3   /* Line-Control-Register                      */
#define UART_MCR     4   /* Modem-Control-Register                     */
#define UART_LSR     5   /* Line-Status-Register                       */
#define UART_MSR     6   /* Modem-Status-Register                      */
#define UART_SCRATCH 7

/* Line Status Register Bits */
#define UART_LSR_DR    (1 << 0)   /* Daten bereit        */
#define UART_LSR_THRE  (1 << 5)   /* TX Holding Register leer */

/* ── Log-Level ───────────────────────────────────────────────────── */
typedef enum log_level {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3,
    LOG_PANIC = 4,
} log_level_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief Initialisiert COM1 mit 115200 Baud, 8N1.
 * @return true wenn der UART erkannt und konfiguriert wurde, sonst false.
 */
bool serial_init(void);

/** Schreibt ein einzelnes Byte (blockiert, bis der TX-Puffer bereit ist). */
void serial_putc(char c);

/** Schreibt einen null-terminierten String. */
void serial_puts(const char *str);

/**
 * @brief Formatierte Ausgabe auf die serielle Schnittstelle (gleiche Spezifizierer wie vga_printf).
 */
void serial_printf(const char *fmt, ...);

/** Liest ein Byte von COM1 (blockierend). */
char serial_getc(void);

/** @return true wenn ein Zeichen im RX-Puffer wartet. */
bool serial_has_data(void);

/* ── Kernel-Log Macros ───────────────────────────────────────────── */
void klog(log_level_t level, const char *fmt, ...);

#define KDBG(fmt, ...)   klog(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define KINFO(fmt, ...)  klog(LOG_INFO,  fmt, ##__VA_ARGS__)
#define KWARN(fmt, ...)  klog(LOG_WARN,  fmt, ##__VA_ARGS__)
#define KERR(fmt, ...)   klog(LOG_ERROR, fmt, ##__VA_ARGS__)

#endif /* _SERIAL_H */
