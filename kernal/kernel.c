/**
 * @file kernel.c
 * @brief PinguinOS – Kernel-Einstiegspunkt und Subsystem-Initialisierung.
 *
 * cmain() wird vom Multiboot-Bootstrap in boot.S aufgerufen.
 * Initialisierungsreihenfolge (kritisch – nicht unbedacht ändern):
 *
 *   1. VGA / Seriell – Ausgabe so früh wie möglich verfügbar
 *   2. GDT           – Segmentregister korrekt konfiguriert
 *   3. IDT / PIC     – Exception- / IRQ-Handling online
 *   4. CPU-Info      – CPUID-Daten gesammelt
 *   5. PMM           – Physische Seitenverwaltung
 *   6. Paging        – MMU aktiviert + Demand-Paging-Handler (#3)
 *   7. Heap          – kmalloc/kfree verfügbar
 *   8. PCI           – Geräte-Enumeration
 *   9. Prozesstabelle – PCB / VMA Infrastruktur
 *  10. Entropy-Pool  – RNG initialisiert (#100)
 *  11. Scheduler     – Präemptives Multitasking mit Priorität (#13)
 *  12. Mouse         – PS/2 IRQ12 Treiber (#32)
 *  13. E1000 NIC     – Netzwerktreiber, falls vorhanden (#44)
 *  14. BGA           – Grafikmodus (optional, falls Hardware vorhanden) (#48)
 *  15. Kernel-Tasks  – Demo-Tasks erstellt, Interrupts aktiviert
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
#include "include/process.h"
#include "include/entropy.h"
#include "include/mouse.h"
#include "include/e1000.h"
#include "include/bga.h"
#include "include/net.h"
#include "include/rtc.h"

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
    vga_puts("  PinguinOS v2.2  |  x86 32-Bit  |  Built with <3\n\n");
}

/* ── Frühe Verzögerung (3 Sekunden) mittels RTC ──────────────────── */
static void early_delay_3s(void)
{
    rtc_time_t t;
    rtc_read(&t);
    uint8_t start_sec = t.second;
    uint32_t elapsed = 0;
    
    /* Warte bis die Sekunde umspringt, um eine volle Sekunde zu haben */
    while (1) {
        rtc_read(&t);
        if (t.second != start_sec) {
            start_sec = t.second;
            break;
        }
    }
    
    /* Jetzt 3 volle Sekunden warten */
    while (elapsed < 3) {
        rtc_read(&t);
        if (t.second != start_sec) {
            elapsed++;
            start_sec = t.second;
        }
    }
}

/* ── Kernel-Einstiegspunkt ───────────────────────────────────────── */
void cmain(uint32_t magic, multiboot_info_t *mbi)
{
    /* ── Schritt 1: Frühe Ausgabe ────────────────────────────────── */
    vga_init();
    serial_init();
    print_banner();
    early_delay_3s();   /* 3-Sekunden-Pause wie vom User gewünscht */
    
    KINFO("PinguinOS bootet...\n");

    /* ── Multiboot-Magic verifizieren ────────────────────────────── */
    if (magic != MULTIBOOT_MAGIC) {
        kpanic("Ungueltiges Multiboot-Magic: 0x%08x (erwartet 0x%08x)\n",
               magic, MULTIBOOT_MAGIC);
    }
    KINFO("Multiboot magic OK  (mbi @ 0x%08x)\n", (uint32_t)mbi);

    /* ── Schritt 2: GDT ──────────────────────────────────────────── */
    vga_printf("  [*] GDT...");
    gdt_init();
    vga_printf(" OK\n");
    KINFO("GDT geladen\n");

    /* ── Schritt 3: IDT / PIC ────────────────────────────────────── */
    vga_printf("  [*] IDT / PIC...");
    idt_init();
    vga_printf(" OK\n");
    KINFO("IDT geladen, PIC auf Vektoren 32–47 gemappt\n");

    /* ── Schritt 4: CPU-Info ─────────────────────────────────────── */
    vga_printf("  [*] CPU...");
    cpu_init();
    vga_printf(" OK\n");
    cpu_dump();
    const cpu_info_t *ci = cpu_get_info();
    vga_printf("      Vendor: %s  |  %s\n", ci->vendor, ci->brand);

    /* ── Schritt 5: Physischer Speichermanager (PMM) ─────────────── */
    vga_printf("  [*] PMM...");
    pmm_init(mbi);
    vga_printf(" OK  (%u MB frei)\n",
               (pmm_free_page_count() * PAGE_SIZE) / (1024 * 1024));

    /* ── Schritt 6: Paging (+ Demand Paging #3) ──────────────────── */
    vga_printf("  [*] Paging (Demand Paging AN)...");
    paging_init();
    vga_printf(" OK\n");

    /* ── Schritt 7: Heap (+ Leak-Detektor #94) ───────────────────── */
    vga_printf("  [*] Heap...");
    heap_init();
    vga_printf(" OK  (bei 0x%08x)\n", KHEAP_START);

    void *p1 = kmalloc(128);
    void *p2 = kzalloc(64);
    if (!p1 || !p2) kpanic("Heap-Selbsttest fehlgeschlagen!\n");
    kfree(p1);
    kfree(p2);
    KINFO("Heap-Selbsttest bestanden\n");

    /* ── Schritt 8: PCI ──────────────────────────────────────────── */
    vga_printf("  [*] PCI...");
    pci_init();
    vga_printf(" OK  (%u Geraet(e))\n", pci_device_count());
    pci_dump();

    /* ── Schritt 9: Prozess-Subsystem ────────────────────────────── */
    vga_printf("  [*] Prozess-Subsystem...");
    proc_init();
    vga_printf(" OK\n");

    /* ── Schritt 10: Entropie / RNG (#100) ────────────────────────── */
    vga_printf("  [*] Entropie-Pool...");
    entropy_init();
    /* Etwas Entropie aus der Boot-Phase mischen */
    entropy_add((uint32_t)mbi);
    entropy_add(magic);
    entropy_add(pmm_free_page_count());
    vga_printf(" OK\n");

    /* ── Schritt 11: Netzwerk-Stack ───────────────────────────────── */
    vga_printf("  [*] Netzwerk-Stack...");
    net_init();
    vga_printf(" OK\n");

    /* ── Schritt 12: Framebuffer (optional) ──────────────────────── */
    if (fb_init(mbi)) {
        KINFO("Framebuffer: %ux%u @ %ubpp\n",
              mbi->framebuffer_width, mbi->framebuffer_height,
              mbi->framebuffer_bpp);
    }

    /* ── Schritt 13: Scheduler (Prioritaets-Scheduling #13) ──────── */
    vga_printf("  [*] Scheduler (Prioritaets-Scheduling AN)...");
    sched_init();
    vga_printf(" OK\n");

    /* ── Schritt 14: PS/2 Maus (#32) ─────────────────────────────── */
    vga_printf("  [*] PS/2 Maus...");
    mouse_init();
    mouse_set_bounds(80, 25);
    vga_printf(" OK\n");

    /* ── Schritt 15: E1000 NIC (#44) ─────────────────────────────── */
    vga_printf("  [*] E1000 NIC...");
    if (e1000_init()) {
        vga_printf(" OK\n");
        /* DHCP wird von einem Kernel-Task erledigt, sobald der Scheduler läuft */
    } else {
        vga_printf(" nicht gefunden\n");
    }

    /* ── Schritt 16: BGA-Grafik (#48) ────────────────────────────── */
    /* Deaktiviert, um im VGA-Textmodus zu bleiben (wie vom User gewünscht) */
    /*
    if (bga_detect()) {
        vga_printf("  [*] BGA: Setze 1024x768x32...");
        if (bga_set_mode(1024, 768, 32)) {
            bga_clear(0x001F2A3Bu);
            vga_printf(" OK\n");
        } else {
            vga_printf(" FEHLER\n");
        }
    }
    */

    /* ── Fertig! Interrupts aktivieren ──────────────────────────── */
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_printf("\n  PinguinOS bereit!  Uptime: %u ms\n", (uint32_t)sched_uptime_ms());
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    KINFO("Alle Subsysteme initialisiert – aktiviere Interrupts\n");
    sched_dump();

    sti();

    for (;;) {
        hlt();
    }
}