/**
 * @file crashdump.c
 * @brief Kernel-Absturzbericht für PinguinOS – Feature #98.
 *
 * Wird automatisch von kpanic() aufgerufen.
 * Schreibt alle verfügbaren Diagnosedaten auf COM1 (seriell)
 * und anschließend in /crash.log auf der RAM-Disk.
 *
 * Bugfixes:
 *   1. rtc_get_time() → rtc_read()  (korrekter Name laut rtc.h)
 *   2. VFS_WRITE  → O_WRONLY        (korrekte Flag laut vfs.h)
 *   3. VFS_CREATE → O_CREAT         (korrekte Flag laut vfs.h)
 */

#include "../include/crashdump.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"
#include "../include/mm.h"
#include "../include/cpu.h"
#include "../include/rtc.h"   /* rtc_time_t, rtc_read() */
#include "../include/vfs.h"   /* O_WRONLY, O_CREAT       */

/* ── Maximale Größe des Dump-Puffers ─────────────────────────── */
#define DUMP_BUF_SIZE   4096

/* ── Globaler Dump-Puffer (statisch, da Heap u.U. korrupt) ────── */
static char dump_buf[DUMP_BUF_SIZE];
static bool dump_schon_geschrieben = false;

/* ── Zeiger auf kpanic-Hook ──────────────────────────────────── */
void (*crashdump_hook)(const char *msg, cpu_regs_t *regs) = NULL;

/* ── Backtrace ausgeben (EBP-Kette) ─────────────────────────── */
static int schreibe_backtrace(char *buf, int maxlen)
{
    int len = 0;
    len += snprintf(buf + len, (uint32_t)(maxlen - len), "  Backtrace:\n");

    uint32_t *ebp = (uint32_t *)__builtin_frame_address(0);
    for (int i = 0; i < 20 && ebp && len < maxlen - 40; i++) {
        /* Sanity-Check: EBP muss im Kernel-Bereich liegen */
        if ((uint32_t)ebp < 0x100000u || (uint32_t)ebp > 0xFFFFFFF0u) break;
        uint32_t ret = *(ebp + 1);
        if (!ret) break;
        len += snprintf(buf + len, (uint32_t)(maxlen - len),
                        "    #%2d  EIP = 0x%08x\n", i, ret);
        ebp = (uint32_t *)*ebp;
    }
    return len;
}

/* ── Öffentlich: crashdump_write ─────────────────────────────── */
void crashdump_write(const char *nachricht, cpu_regs_t *regs)
{
    /* Nur einmal schreiben (Doppel-Panic vermeiden) */
    if (dump_schon_geschrieben) return;
    dump_schon_geschrieben = true;

    int pos = 0;
    int max = DUMP_BUF_SIZE - 1;

    /* ── Kopfzeile ───────────────────────────────────────────── */
    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
        "══════════════════════════════════════════════════════\n"
        "  PINGUINOS 2.3 KERNEL-ABSTURZBERICHT\n"
        "══════════════════════════════════════════════════════\n");

    /* ── RTC-Zeitstempel ─────────────────────────────────────── */
    rtc_time_t zeit;
    rtc_read(&zeit);   /* Fix: rtc_read() statt rtc_get_time() */
    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
                    "  Zeitpunkt: %04u-%02u-%02u %02u:%02u:%02u\n",
                    (uint32_t)(2000 + zeit.year),
                    zeit.month, zeit.day,
                    zeit.hour, zeit.minute, zeit.second);

    /* ── Fehlermeldung ───────────────────────────────────────── */
    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
                    "  Fehler:    %s\n",
                    nachricht ? nachricht : "(unbekannt)");

    /* ── Register ────────────────────────────────────────────── */
    if (regs) {
        pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
            "  Register:\n"
            "    EAX=0x%08x  EBX=0x%08x  ECX=0x%08x  EDX=0x%08x\n"
            "    ESI=0x%08x  EDI=0x%08x  EBP=0x%08x  ESP=0x%08x\n"
            "    EIP=0x%08x  EFLAGS=0x%08x\n"
            "    INT=%u       ERR=0x%08x\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx,
            regs->esi, regs->edi, regs->ebp, regs->esp,
            regs->eip, regs->eflags,
            regs->int_no, regs->err_code);
    }

    /* ── Speicher ─────────────────────────────────────────────── */
    uint32_t frei_kb = pmm_free_page_count() * PAGE_SIZE / 1024;
    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
                    "  Freier Speicher: %u KB\n", frei_kb);

    /* ── Laufzeit ─────────────────────────────────────────────── */
    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
                    "  Laufzeit: %u ms\n  Tasks: %u\n",
                    sched_uptime_ms(), sched_task_count());

    /* ── Backtrace ───────────────────────────────────────────── */
    pos += schreibe_backtrace(dump_buf + pos, max - pos);

    pos += snprintf(dump_buf + pos, (uint32_t)(max - pos),
        "══════════════════════════════════════════════════════\n");

    dump_buf[pos] = '\0';

    /* ── Auf seriell ausgeben ─────────────────────────────────── */
    serial_puts(dump_buf);

    /* ── In /crash.log schreiben (falls VFS verfügbar) ────────── */
    /* Fix: O_WRONLY | O_CREAT statt VFS_WRITE | VFS_CREATE       */
    vfs_node_t *node = vfs_open("/crash.log", O_WRONLY | O_CREAT);
    if (node) {
        vfs_write(node, 0, (uint32_t)pos, (uint8_t *)dump_buf);
        vfs_close(node);
    }
}

/* ── Öffentlich: crashdump_init ──────────────────────────────── */
void crashdump_init(void)
{
    crashdump_hook         = crashdump_write;
    dump_schon_geschrieben = false;
    KINFO("Crash-Dump: initialisiert  (Hook gesetzt)\n");
}