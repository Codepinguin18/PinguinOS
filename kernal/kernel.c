/**
 * @file kernel.c
 * @brief PinguinOS 2.3 – Kernel-Einstiegspunkt und Subsystem-Initialisierung.
 *
 * cmain() wird vom Multiboot-Bootstrap in boot.S aufgerufen.
 *
 * Initialisierungsreihenfolge (NICHT beliebig ändern):
 *
 *   1.  VGA / Serial       – frühestmögliche Ausgabe
 *   2.  GDT                – Segmentregister korrekt konfiguriert
 *   3.  IDT / PIC          – Ausnahmen und IRQ-Handler aktiv
 *   4.  CPU-Info           – CPUID-Daten gesammelt
 *   5.  PMM                – physische Seiten verwaltet
 *   6.  Paging             – MMU aktiv
 *   7.  Heap               – kmalloc/kfree verfügbar
 *   8.  Entropy / RNG      – Zufallspool initialisiert
 *   9.  SSP                – Stack-Canary gesetzt
 *  10.  PCI                – Geräte aufgezählt
 *  11.  APIC               – Local APIC aktiviert (falls verfügbar)
 *  12.  CPU-Topologie      – Kern-/Thread-Erkennung
 *  13.  IDE                – ATA-Festplatten
 *  14.  AHCI               – SATA-Controller (falls vorhanden)
 *  15.  RTL8139            – alternative NIC (falls vorhanden)
 *  16.  Seitenpool         – Seiten-Cache für Block-I/O
 *  17.  VFS + Dateisysteme – ramfs / fat32 / devfs / procfs / iso9660 / sysfs
 *  18.  Initrd             – RAM-Disk entladen (falls vorhanden)
 *  19.  Swap               – Auslagerungspartition
 *  20.  Prozess-Subsystem  – proc_init()
 *  21.  IPC                – Pipes / Env / Shmem / Msgqueue / Unix-Sockets
 *  22.  Netzwerk           – Ethernet-Stack + Loopback
 *  23.  TCP                – TCP-Schicht
 *  24.  E1000-NIC          – falls vorhanden
 *  25.  DHCP               – IP-Adresse holen
 *  26.  GDB-Stub           – Ferndebugging über serielle Schnittstelle
 *  27.  Profiler           – Kernel-Sampling-Profiler
 *  28.  Kprobes            – dynamisches Tracing
 *  29.  Crash-Dumps        – automatische Absturzberichte
 *  30.  Scheduler          – preemptives Multitasking (1 kHz, MLFQ)
 *  31.  Watchdog           – Neustart bei Absturz (10 s Timeout)
 *  32.  BGA-Grafik         – optionaler Framebuffer
 *  33.  Debug-Shell        – interaktive Kernel-Konsole
 *  34.  Demo-Tasks         – STI → Idle-Schleife
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
#include "include/ide.h"
#include "include/scheduler.h"
#include "include/klib.h"
#include "include/entropy.h"
#include "include/net.h"
#include "include/vfs.h"
#include "include/process.h"
#include "include/keyboard.h"
#include "include/mouse.h"
#include "include/rtc.h"
#include "include/acpi.h"
#include "include/debug_shell.h"
#include "include/watchdog.h"
#include "include/bga.h"
#include "include/tcp.h"
#include "include/ssp.h"
#include "include/partition.h"
#include "include/dns.h"
#include "include/kprobes.h"
/* Neue Subsysteme (diese Session) */
#include "include/ahci.h"
#include "include/rtl8139.h"
#include "include/apic.h"
#include "include/cpu_topo.h"
#include "include/swap.h"
#include "include/sysfs.h"
#include "include/initrd.h"
#include "include/gdbstub.h"
#include "include/profiler.h"
#include "include/crashdump.h"
#include "include/unix_sock.h"
#include "include/pagecache.h"
#include "include/pipe.h"
#include "include/env.h"
#include "include/shmem.h"
#include "include/msgqueue.h"
/* PinguinOS Desktop-Umgebung */
#include "../os/include/pinguinos.h"

/* ── Kernel-Version ──────────────────────────────────────────────── */
#define KERNEL_VERSION  "2.3"

/* ── RTC-Ports für Früh-Delay ────────────────────────────────────── */
#define RTC_PORT_CMD   0x70
#define RTC_PORT_DATA  0x71
#define RTC_REG_SEC    0x00

/* ── Hilfsmakro: Subsystem-Ausgabe ───────────────────────────────── */
#define KINIT(bezeichnung, ausdruck) do { \
    vga_printf("  [*] %-22s", bezeichnung); \
    (ausdruck); \
    vga_printf(" OK\n"); \
    KINFO("%-22s initialisiert\n", bezeichnung); \
} while(0)

/* ─────────────────────────────────────────────────────────────────
 * early_delay_sec – Wartezeit VOR dem Scheduler über RTC-Polling.
 *
 * Liest Register 0 der CMOS-Echtzeituhr (RTC) und wartet, bis sich
 * der Sekundenwert @p sekunden-mal geändert hat.  Keine Interrupts
 * nötig – funktioniert ab dem allerersten Boot-Moment.
 * ─────────────────────────────────────────────────────────────────*/
static void early_delay_sec(uint32_t sekunden)
{
    for (uint32_t i = 0; i < sekunden; i++) {
        /* Aktuellen Sekundenwert lesen */
        outb(RTC_PORT_CMD,  RTC_REG_SEC);
        uint8_t vorher = inb(RTC_PORT_DATA);
        uint8_t jetzt;
        /* Warten bis sich der Sekundenwert ändert (~1 Sekunde) */
        do {
            outb(RTC_PORT_CMD,  RTC_REG_SEC);
            jetzt = inb(RTC_PORT_DATA);
        } while (jetzt == vorher);
    }
}

/* ─────────────────────────────────────────────────────────────────
 * print_banner – Splash-Screen mit PinguinOS + PinguinOS 2.3
 * ─────────────────────────────────────────────────────────────────*/
static void print_banner(void)
{
    /* Bildschirm leeren */
    vga_clear();

    /* PinguinOS – ASCII-Art, Cyan */
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts(
        " ____  _                   _        ___  ____  \n"
        "|  _ \\(_)_ __   __ _ _   _(_)_ __  / _ \\/ ___| \n"
        "| |_) | | '_ \\ / _` | | | | | '_ \\| | | \\___ \\ \n"
        "|  __/| | | | | (_| | |_| | | | | | |_| |___) |\n"
        "|_|   |_|_| |_|\\__, |\\__,_|_|_| |_|\\___/|____/ \n"
        "                |___/                           \n"
    );

    /* Trennlinie */
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts(
        "  ────────────────────────────────────────────────────────────────────────── \n"
    );

    /* PinguinOS Versionszeile – Gelb */
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    vga_puts(
        "                  PinguinOS 2.3  |  x86-32  |  Build with <3               \n"
    );

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts(
        "  ────────────────────────────────────────────────────────────────────────── \n"
    );

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    /* 2 Sekunden Pause – Splash wird sichtbar bevor Konsolenausgabe startet */
    early_delay_sec(2);
}

/* ─────────────────────────────────────────────────────────────────
 *  Watchdog-Task – petzt den Watchdog alle 2 Sekunden
 * ─────────────────────────────────────────────────────────────────*/
static void watchdog_task(void)
{
    for (;;) {
        watchdog_pet();
        task_sleep(2000);
    }
}

/* ─────────────────────────────────────────────────────────────────
 *  Netzwerk-Init-Task – läuft nach dem Scheduler-Start
 * ─────────────────────────────────────────────────────────────────*/
static void net_init_task(void)
{
    /* Loopback-Interface registrieren */
    extern void loopback_init(void);
    loopback_init();

    /* E1000 und RTL8139 versuchen */
    e1000_init();
    rtl8139_init();

    /* DHCP auf primärer Schnittstelle */
    net_iface_t *iface = net_get_primary();
    if (iface && iface->up) {
        KINFO("Netz: DHCP auf %s...\n", iface->name);
        extern bool dhcp_discover(net_iface_t *, uint32_t);
        if (dhcp_discover(iface, 5000)) {
            KINFO("Netz: IP %u.%u.%u.%u\n",
                  (iface->ip>>24)&0xFF, (iface->ip>>16)&0xFF,
                  (iface->ip>>8)&0xFF,  iface->ip&0xFF);
            dns_set_server(iface->gateway);
        } else {
            KERR("Netz: DHCP fehlgeschlagen\n");
        }
    }

    tcp_init();
    task_exit(0);
}

/* ─────────────────────────────────────────────────────────────────
 *  Demo-Task – schreibt regelmäßig auf die serielle Schnittstelle
 * ─────────────────────────────────────────────────────────────────*/
static void demo_task(void)
{
    uint32_t n = 0;
    for (;;) {
        char buf[72];
        snprintf(buf, sizeof(buf),
                 "[Demo] Tick %u  Laufzeit %u ms\n", n++, sched_uptime_ms());
        serial_puts(buf);
        task_sleep(1000);
    }
}

/* ─────────────────────────────────────────────────────────────────
 *  Debug-Shell-Task – blockiert und liest von Seriell/Tastatur
 * ─────────────────────────────────────────────────────────────────*/
static void debug_shell_task(void)
{
    debug_shell_run();
}

/* ══════════════════════════════════════════════════════════════════
 *  cmain – C-Einstiegspunkt, aufgerufen von boot.S
 * ══════════════════════════════════════════════════════════════════ */
void cmain(uint32_t magic, multiboot_info_t *mbi)
{
    /* ── Schritt 1: Frühausgabe ───────────────────────────────────── */
    vga_init();
    serial_init();

    /* Splash anzeigen und 2 Sekunden warten */
    print_banner();

    /* Ab hier erscheint der Konsolen-Output */
    KINFO("PinguinOS %s startet...\n", KERNEL_VERSION);

    /* Multiboot-Magic prüfen */
    if (magic != MULTIBOOT_MAGIC)
        kpanic("Ungültiger Multiboot-Magic: 0x%08x\n", magic);
    KINFO("Multiboot OK  (mbi @ 0x%08x)\n", (uint32_t)mbi);

    /* ── Schritt 2: GDT ──────────────────────────────────────────── */
    KINIT("GDT", gdt_init());

    /* ── Schritt 3: IDT / PIC ────────────────────────────────────── */
    KINIT("IDT/PIC", idt_init());

    /* ── Schritt 4: CPU-Info ─────────────────────────────────────── */
    cpu_init();
    {
        const cpu_info_t *ci = cpu_get_info();
        vga_printf("      CPU: %s\n", ci->brand);
        vga_printf("           %s  Fam=%u Mod=%u Step=%u\n",
                   ci->vendor, ci->family, ci->model, ci->stepping);
    }

    /* ── Schritt 5: PMM ──────────────────────────────────────────── */
    pmm_init(mbi);
    vga_printf("  [*] %-22s OK  (%u MB frei)\n", "PMM",
               (pmm_free_page_count() * PAGE_SIZE) / (1024 * 1024));

    /* ── Schritt 6: Paging ───────────────────────────────────────── */
    KINIT("Paging", paging_init());

    /* ── Schritt 7: Heap ─────────────────────────────────────────── */
    heap_init();
    {
        /* Schneller Selbsttest */
        void *p1 = kmalloc(256), *p2 = kzalloc(128);
        if (!p1 || !p2) kpanic("Heap-Selbsttest fehlgeschlagen!\n");
        kfree(p1); kfree(p2);
    }
    vga_printf("  [*] %-22s OK  (@ 0x%08x)\n", "Heap", KHEAP_START);

    /* ── Schritt 8: Entropy / RNG ────────────────────────────────── */
    entropy_init();
    entropy_add((uint32_t)mbi ^ 0xDEADBEEF);
    vga_printf("  [*] %-22s OK\n", "Entropy/RNG");

    /* ── Schritt 9: SSP (Stack-Canary) ──────────────────────────── */
    KINIT("SSP (Stack-Canary)", ssp_init());

    /* ── Schritt 10: PCI ─────────────────────────────────────────── */
    pci_init();
    vga_printf("  [*] %-22s OK  (%u Gerät(e))\n", "PCI", pci_device_count());

    /* ── Schritt 11: Local APIC ──────────────────────────────────── */
    if (apic_detect()) {
        KINIT("Local APIC", apic_init());
    } else {
        vga_printf("  [*] %-22s nicht vorhanden (PIC-Modus)\n", "Local APIC");
    }

    /* ── Schritt 12: CPU-Topologie ───────────────────────────────── */
    KINIT("CPU-Topologie", cpu_topo_init());
    {
        const cpu_topo_t *topo = cpu_topo_get();
        vga_printf("      %u Kern(e), %u logische CPU(s)\n",
                   topo->physical_cores, topo->logical_cpus);
    }

    /* ── Schritt 13: IDE + Partitionen ──────────────────────────── */
    ide_init();
    {
        partition_table_t pt;
        if (partition_read(0, &pt) == 0)
            vga_printf("  [*] %-22s OK  (%s, %u Partition(en))\n",
                       "IDE/Partitionen", pt.is_gpt ? "GPT" : "MBR", pt.count);
        else
            vga_printf("  [*] %-22s OK  (keine Disk)\n", "IDE");
    }

    /* ── Schritt 14: AHCI/SATA ───────────────────────────────────── */
    {
        int ahci_drives = ahci_init();
        if (ahci_drives > 0)
            vga_printf("  [*] %-22s OK  (%d Laufwerk(e))\n",
                       "AHCI/SATA", ahci_drives);
        else
            vga_printf("  [*] %-22s nicht vorhanden\n", "AHCI/SATA");
    }

    /* ── Schritt 15: Seiten-Cache ────────────────────────────────── */
    KINIT("Seiten-Cache", pagecache_init());

    /* ── Schritt 16: VFS + Dateisysteme ──────────────────────────── */
    vfs_init();
    vfs_register_fs(&ramfs_ops);
    vfs_register_fs(&fat32_ops);
    vfs_register_fs(&devfs_ops);
    vfs_register_fs(&procfs_ops);
    vfs_register_fs(&iso9660_ops);
    vfs_register_fs(&sysfs_ops);
    vfs_mount("/",         NULL,     "ramfs",   0);
    devfs_init();
    vfs_mount("/dev",      NULL,     "devfs",   0);
    vfs_mount("/proc",     NULL,     "procfs",  0);
    vfs_mount("/sys",      NULL,     "sysfs",   0);
    vfs_mount("/mnt/disk", "ide0p0", "fat32",   0);
    vga_printf("  [*] %-22s OK\n", "VFS/Dateisysteme");

    /* ── Schritt 17: Initrd entladen ─────────────────────────────── */
    if (mbi->flags & (1 << 3) && mbi->mods_count > 0) {
        initrd_init(mbi->mods_addr);
        KINFO("Initrd: RAM-Disk eingebunden\n");
        vga_printf("  [*] %-22s OK\n", "Initrd");
    }

    /* ── Schritt 18: Swap ────────────────────────────────────────── */
    /* Swap auf zweiter IDE-Partition, falls vorhanden */
    swap_init(0, 0x800, 0x8000);   /* LBA 2048, 32768 Sektoren = 16 MB */
    if (swap_ready_p())
        vga_printf("  [*] %-22s OK\n", "Swap");

    /* ── Schritt 19: Prozess-Subsystem ──────────────────────────── */
    KINIT("Prozess-Manager", proc_init());

    /* ── Schritt 20: IPC-Subsysteme ──────────────────────────────── */
    pipe_init();
    env_init();
    shmem_init();
    mq_init();
    unix_sock_init();
    vga_printf("  [*] %-22s OK  (Pipe/Env/SHM/MQ/Unix)\n", "IPC");

    /* ── Schritt 21: Netzwerk-Stack ──────────────────────────────── */
    KINIT("Netzwerk-Stack", net_init());

    /* ── Schritt 22: Tastatur + Maus ─────────────────────────────── */
    KINIT("Tastatur",  kbd_init());
    KINIT("Maus",      mouse_init());

    /* ── Schritt 23: RTC ─────────────────────────────────────────── */
    KINIT("RTC (Echtzeituhr)", rtc_init());

    /* ── Schritt 24: ACPI ────────────────────────────────────────── */
    KINIT("ACPI", acpi_init());

    /* ── Schritt 25: Debugging-Werkzeuge ─────────────────────────── */
    KINIT("Kprobes",     kprobes_init());
    KINIT("GDB-Stub",    gdbstub_init());
    KINIT("Profiler",    profiler_init());
    KINIT("Crash-Dumps", crashdump_init());

    /* ── Schritt 26: Scheduler (v2.3, 1 kHz, O(1)-MLFQ) ────────── */
    sched_init();
    vga_printf("  [*] %-22s OK  (1 kHz  O(1)-MLFQ)\n", "Scheduler v2.3");

    /* ── Schritt 27: Watchdog (10 Sekunden) ──────────────────────── */
    watchdog_init(10);

    /* ── Schritt 28: BGA-Grafik (optional) ──────────────────────── */
    if (bga_detect()) {
        bga_set_mode(1280, 720, 32);
        bga_clear(0x001B2A3C);   /* Dunkles PinguinOS-Blau */
        vga_printf("  [*] %-22s OK  (1024×768×32bpp)\n", "BGA-Grafik");
    }

    /* ── Schritt 29: Kernel-Tasks erstellen ──────────────────────── */
    task_create_prio(watchdog_task,     "watchdog",    TASK_PRIO_RT);
    task_create_prio(net_init_task,     "net_init",    TASK_PRIO_HIGH);
    task_create_prio(debug_shell_task,  "debug_shell", TASK_PRIO_NORMAL);
    task_create_prio(demo_task,         "demo",        TASK_PRIO_LOW);

    /* ── Boot abgeschlossen ──────────────────────────────────────── */
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_printf("\n  PinguinOS %s bereit!\n", KERNEL_VERSION);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    KINFO("Alle Subsysteme gestartet – Interrupts werden aktiviert\n");
    sched_dump();

    /* Watchdog scharf stellen und Interrupts freigeben */
    watchdog_enable();
    sti();

    /* ── PinguinOS Desktop-Umgebung starten ──────────────────────── */
    pinguinos_main();   /* Login → Desktop (kehrt nur bei Shutdown zurück) */

    /* Idle-Schleife – der PIT-IRQ treibt den Scheduler */
    for (;;) hlt();
}