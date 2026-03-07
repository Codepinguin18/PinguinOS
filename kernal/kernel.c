/**
 * @file kernel.c
 * @brief PinguinOS – Kernel-Einstiegspunkt und Subsystem-Initialisierung.
 *
 * cmain() wird vom Multiboot-Bootstrap in boot.S aufgerufen.
 * Initialisierungs-Reihenfolge (kritisch – nicht unachtsam umstellen):
 *
 *   1. VGA / Serial  – Ausgabe so früh wie möglich verfügbar
 *   2. GDT           – Segmentregister korrekt konfiguriert
 *   3. IDT / PIC     – Exception- / IRQ-Behandlung online
 *   4. CPU Info      – CPUID-Daten gesammelt
 *   5. PMM           – Physische Seiten getrackt
 *   6. Paging        – MMU aktiviert
 *   7. Heap          – kmalloc/kfree verfügbar
 *   8. PCI           – Geräte-Enumeration
 *   9. Scheduler     – Präemptives Multitasking beginnt
 *  10. Kernel-Tasks  – Demo-Tasks erstellt, Interrupts aktiviert
 */

#include "include/types.h"
#include "include/multiboot.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/cpu.h"
#include "include/mm.h"
#include "include/paging.h"
#include "include/heap.h"
#include "include/vga.h"
#include "include/serial.h"
#include "include/pci.h"
#include "include/scheduler.h"
#include "include/klib.h"

/* ── Banner ──────────────────────────────────────────────────────── */
static void print_banner(void)
{
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts(
        " ____  _                   _        ___  ____  \n"
        "|  _ \\(_)_ __   __ _ _   _(_)_ __  / _ \\/ ___| \n"
        "| |_) | | '_ \\ / _` | | | | | '_ \\| | | \\___ \\ \n"
        "|  __/| | | | | (_| | |_| | | | | | |_| |___) |\n"
        "|_|   |_|_| |_|\\__, |\\__,_|_|_| |_|\\___/|____/ \n"
        "                |___/                           \n"
    );
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_puts("  PinguinOS v0.1.0  |  x86 32-bit  |  Entwickelt mit <3\n\n");
}

/* ── Demo-Task: Lässt einen Zähler auf dem Bildschirm blinken ────── */
static void demo_task_a(void)
{
    uint32_t count = 0;
    for (;;) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[Task A] count = %u\n", count++);
        serial_puts(buf);
        task_sleep(500);
    }
}

static void demo_task_b(void)
{
    uint32_t count = 0;
    for (;;) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[Task B] count = %u\n", count++);
        serial_puts(buf);
        task_sleep(750);
    }
}

/* ── Kernel-Einstiegspunkt ───────────────────────────────────────── */
/**
 * @brief C-Einstiegspunkt, aufgerufen von boot.S nach Einrichtung des Stacks.
 *
 * @param magic  Muss MULTIBOOT_MAGIC (0x2BADB002) entsprechen.
 * @param mbi    Physische Adresse der multiboot_info_t Struktur.
 */
void cmain(uint32_t magic, multiboot_info_t *mbi)
{
    /* ── Schritt 1: Frühe Ausgabe ─────────────────────────────────── */
    vga_init();
    serial_init();

    print_banner();
    KINFO("PinguinOS bootet...\n");

    /* ── Multiboot-Magic verifizieren ─────────────────────────────── */
    if (magic != MULTIBOOT_MAGIC) {
        kpanic("Falsche Multiboot-Magic: 0x%08x (erwartet 0x%08x)\n",
               magic, MULTIBOOT_MAGIC);
    }
    KINFO("Multiboot-Magic OK  (mbi @ 0x%08x)\n", (uint32_t)mbi);

    /* ── Schritt 2: GDT ────────────────────────────────────────────── */
    vga_printf("  [*] GDT...");
    gdt_init();
    vga_printf(" OK\n");
    KINFO("GDT geladen\n");

    /* ── Schritt 3: IDT / PIC ─────────────────────────────────────── */
    vga_printf("  [*] IDT / PIC...");
    idt_init();
    vga_printf(" OK\n");
    KINFO("IDT geladen, PIC auf Vektoren 32-47 remapped\n");

    /* ── Schritt 4: CPU-Info ───────────────────────────────────────── */
    vga_printf("  [*] CPU...");
    cpu_init();
    vga_printf(" OK\n");
    cpu_dump();

    const cpu_info_t *ci = cpu_get_info();
    vga_printf("      Vendor: %s  |  %s\n", ci->vendor, ci->brand);

    /* ── Schritt 5: Physischer Speichermanager (PMM) ──────────────── */
    vga_printf("  [*] PMM...");
    pmm_init(mbi);
    vga_printf(" OK  (%u MB frei)\n",
               (pmm_free_page_count() * PAGE_SIZE) / (1024 * 1024));

    /* ── Schritt 6: Paging ────────────────────────────────────────── */
    vga_printf("  [*] Paging...");
    paging_init();
    vga_printf(" OK\n");

    /* ── Schritt 7: Heap ──────────────────────────────────────────── */
    vga_printf("  [*] Heap...");
    heap_init();
    vga_printf(" OK  (bei 0x%08x)\n", KHEAP_START);

    /* Kurzer Heap-Selbsttest */
    void *p1 = kmalloc(128);
    void *p2 = kzalloc(64);
    if (!p1 || !p2) kpanic("Heap-Selbsttest fehlgeschlagen!\n");
    kfree(p1);
    kfree(p2);
    KINFO("Heap-Selbsttest bestanden\n");

    /* ── Schritt 8: PCI ────────────────────────────────────────────── */
    vga_printf("  [*] PCI...");
    pci_init();
    vga_printf(" OK  (%u Gerät(e))\n", pci_device_count());
    pci_dump();

    /* ── Schritt 9: Framebuffer (optional) ────────────────────────── */
    if (fb_init(mbi)) {
        KINFO("Framebuffer: %ux%u @ %ubpp\n",
              mbi->framebuffer_width, mbi->framebuffer_height,
              mbi->framebuffer_bpp);
    }

    /* ── Schritt 10: Scheduler ─────────────────────────────────────── */
    vga_printf("  [*] Scheduler...");
    sched_init();
    vga_printf(" OK\n");

    /* ── Demo-Tasks erstellen ─────────────────────────────────────── */
    task_create(demo_task_a, "demo_a");
    task_create(demo_task_b, "demo_b");

    /* ── Alles erledigt – Interrupts aktivieren und in Idle-Schleife fallen ── */
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_printf("\n  PinguinOS bereit!  Uptime: %u ms\n", sched_uptime_ms());
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    KINFO("Alle Subsysteme initialisiert – Interrupts werden aktiviert\n");
    sched_dump();

    /* Interrupts aktivieren; der PIT steuert nun den Scheduler */
    sti();

    /*
     * Die Startsequenz übergibt hier an den Idle-Task.
     * In einem echten Kernel würden wir hier in den User-Space oder die Shell wechseln.
     * Vorerst nur eine Endlosschleife; der Timer-IRQ hält das System am Laufen.
     */
    for (;;) {
        hlt();
    }
}
