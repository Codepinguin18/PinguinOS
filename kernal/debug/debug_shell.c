/**
 * @file debug_shell.c
 * @brief Interaktive Kernel-Debug-Shell für PinguinOS.
 */
#include "../include/debug_shell.h"
#include "../include/serial.h"
#include "../include/vga.h"
#include "../include/mm.h"
#include "../include/heap.h"
#include "../include/paging.h"
#include "../include/pci.h"
#include "../include/scheduler.h"
#include "../include/net.h"
#include "../include/vfs.h"
#include "../include/acpi.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/keyboard.h"

/* ── Eingabe-Puffer ──────────────────────────────────────────────── */
#define SHELL_BUF   256
#define SHELL_HIST   8

static char hist[SHELL_HIST][SHELL_BUF];
static int  hist_count = 0;
static int  hist_idx   = 0;

/* ── Hilfsfunktion: Hex-Dump ─────────────────────────────────────── */
static void hexdump(uint32_t addr, uint32_t len)
{
    const uint8_t *p = (const uint8_t *)addr;
    for (uint32_t i = 0; i < len; i += 16) {
        serial_printf("  %08x: ", addr + i);
        for (uint32_t j = 0; j < 16 && i + j < len; j++)
            serial_printf("%02x ", p[i + j]);
        serial_puts(" | ");
        for (uint32_t j = 0; j < 16 && i + j < len; j++)
            serial_putc((p[i + j] >= 32 && p[i + j] < 127) ? p[i + j] : '.');
        serial_puts("\n");
    }
}

/* ── Befehlsverarbeitung ─────────────────────────────────────────── */
static void shell_exec(const char *line)
{
    /* Leerzeilen überspringen */
    while (*line == ' ') line++;
    if (!*line) return;

    /* In Verlauf eintragen */
    strncpy(hist[hist_idx % SHELL_HIST], line, SHELL_BUF - 1);
    hist_idx++;
    if (hist_count < SHELL_HIST) hist_count++;

    /* Befehl parsen */
    char cmd[64];
    const char *args = line;
    size_t i = 0;
    while (*args && *args != ' ' && i < 63) cmd[i++] = *args++;
    cmd[i] = '\0';
    while (*args == ' ') args++;

    if (strcmp(cmd, "help") == 0) {
        serial_puts(
            "PinguinOS Debug-Shell\n"
            "  help              – diese Hilfe\n"
            "  mem               – Speicherinfo\n"
            "  ps                – Task-Liste\n"
            "  pci               – PCI-Geräte\n"
            "  vfs               – Mount-Tabelle\n"
            "  net               – Netzwerk-Statistiken\n"
            "  reboot            – Neustart\n"
            "  shutdown          – Herunterfahren\n"
            "  echo <text>       – Text ausgeben\n"
            "  hexdump <a> <n>   – Speicher-Dump\n"
            "  mmap <virt>       – physische Adresse\n"
            "  uptime            – Betriebszeit\n"
            "  rtc               – Uhrzeit\n"
            "  panic             – Test-Panic\n"
            "  clear             – VGA leeren\n"
        );
    }
    else if (strcmp(cmd, "mem") == 0) {
        uint32_t free_pages  = pmm_free_page_count();
        uint32_t total_pages = pmm_total_page_count();
        serial_printf("PMM: %u/%u Seiten frei (%u/%u MB)\n",
                      free_pages, total_pages,
                      (free_pages  * PAGE_SIZE) / (1024*1024),
                      (total_pages * PAGE_SIZE) / (1024*1024));
        serial_printf("Heap: %u KB benutzt  %u KB frei\n",
                      (uint32_t)(heap_used()      / 1024),
                      (uint32_t)(heap_available() / 1024));
    }
    else if (strcmp(cmd, "ps") == 0) {
        sched_dump();
    }
    else if (strcmp(cmd, "pci") == 0) {
        pci_dump();
    }
    else if (strcmp(cmd, "vfs") == 0) {
        vfs_dump();
    }
    else if (strcmp(cmd, "net") == 0) {
        net_dump();
    }
    else if (strcmp(cmd, "uptime") == 0) {
        uint32_t ms = sched_uptime_ms();
        serial_printf("Betriebszeit: %u ms (%u s)\n", ms, ms / 1000);
    }
    else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
    }
    else if (strcmp(cmd, "echo") == 0) {
        serial_puts(args);
        serial_putc('\n');
        vga_printf("%s\n", args);
    }
    else if (strcmp(cmd, "hexdump") == 0) {
        uint32_t addr = 0, len = 64;
        /* Vereinfachtes Hex-Parsing */
        if (*args) {
            const char *p = args;
            while (*p) { addr = addr * 16 + (*p <= '9' ? *p-'0' : *p-'a'+10); p++; if (*p == ' ') { p++; break; } }
            while (*p) { len  = len  * 10 + (*p - '0'); p++; }
        }
        hexdump(addr, len ? len : 64);
    }
    else if (strcmp(cmd, "mmap") == 0) {
        uint32_t virt = 0;
        for (const char *p = args; *p; p++)
            virt = virt * 16 + (*p <= '9' ? *p-'0' : *p-'a'+10);
        uint32_t phys = paging_get_phys(virt);
        serial_printf("virt 0x%08x → phys 0x%08x\n", virt, phys);
    }
    else if (strcmp(cmd, "reboot") == 0) {
        acpi_reboot();
    }
    else if (strcmp(cmd, "shutdown") == 0) {
        acpi_shutdown();
    }
    else if (strcmp(cmd, "panic") == 0) {
        kpanic("Manuell ausgelöste Test-Panic aus der Debug-Shell!\n");
    }
    else {
        serial_printf("Unbekannter Befehl: '%s'  (help für Hilfe)\n", cmd);
    }
}

/* ── Zeile einlesen (über Serial COM1) ──────────────────────────── */
static void shell_readline(char *buf, size_t max)
{
    size_t pos = 0;
    buf[0] = '\0';

    for (;;) {
        char c = serial_getc();

        if (c == '\r' || c == '\n') {
            buf[pos] = '\0';
            serial_puts("\n");
            return;
        }
        if ((c == '\b' || c == 127) && pos > 0) {
            pos--;
            serial_puts("\b \b");
            continue;
        }
        if (c >= 32 && c < 127 && pos + 1 < max) {
            buf[pos++] = c;
            serial_putc(c);   /* Echo */
        }
    }
}

/* ── Haupt-Shell-Schleife ────────────────────────────────────────── */
void debug_shell_run(void)
{
    char buf[SHELL_BUF];

    serial_puts("\n\n");
    serial_puts("╔══════════════════════════════════════╗\n");
    serial_puts("║   PinguinOS Debug-Shell v1.0         ║\n");
    serial_puts("║   'help' für Befehlsliste            ║\n");
    serial_puts("╚══════════════════════════════════════╝\n");

    for (;;) {
        serial_printf("\npinguin@kernel:%u$ ", sched_uptime_ms() / 1000);
        shell_readline(buf, sizeof(buf));
        shell_exec(buf);
    }
}

/* ── Als Task starten ────────────────────────────────────────────── */
static void shell_task_fn(void)
{
    debug_shell_run();
}

void debug_shell_init(void)
{
    task_create(shell_task_fn, "debug_shell");
    KINFO("Debug-Shell: als Task gestartet (liest von COM1)\n");
}
