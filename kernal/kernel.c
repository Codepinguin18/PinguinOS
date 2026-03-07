/**
 * @file kernel.c
 * @brief PinguinOS – kernel entry point and subsystem initialisation.
 *
 * cmain() is called by the Multiboot bootstrap in boot.S.
 * Initialisation order (critical – do not reorder carelessly):
 *
 *   1. VGA / serial  – output available as early as possible
 *   2. GDT           – segment registers properly configured
 *   3. IDT / PIC     – exception / IRQ handling online
 *   4. CPU info      – CPUID data collected
 *   5. PMM           – physical pages tracked
 *   6. Paging        – MMU enabled
 *   7. Heap          – kmalloc/kfree available
 *   8. PCI           – device enumeration
 *   9. Scheduler     – preemptive multitasking begins
 *  10. Kernel tasks  – demo tasks created, interrupts enabled
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
    vga_puts("  PinguinOS v2.1  |  x86 32-bit  |  Built with <3\n\n");
}

/* ── Demo task: blinks a counter on screen ───────────────────────── */
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

/* ── Kernel entry point ──────────────────────────────────────────── */
/**
 * @brief C entry point called by boot.S after setting up the stack.
 *
 * @param magic  Must equal MULTIBOOT_MAGIC (0x2BADB002).
 * @param mbi    Physical address of the multiboot_info_t structure.
 */
void cmain(uint32_t magic, multiboot_info_t *mbi)
{
    /* ── Step 1: Early output ─────────────────────────────────────── */
    vga_init();
    serial_init();

    print_banner();
    KINFO("PinguinOS booting...\n");

    /* ── Verify Multiboot magic ──────────────────────────────────── */
    if (magic != MULTIBOOT_MAGIC) {
        kpanic("Bad Multiboot magic: 0x%08x (expected 0x%08x)\n",
               magic, MULTIBOOT_MAGIC);
    }
    KINFO("Multiboot magic OK  (mbi @ 0x%08x)\n", (uint32_t)mbi);

    /* ── Step 2: GDT ─────────────────────────────────────────────── */
    vga_printf("  [*] GDT...");
    gdt_init();
    vga_printf(" OK\n");
    KINFO("GDT loaded\n");

    /* ── Step 3: IDT / PIC ───────────────────────────────────────── */
    vga_printf("  [*] IDT / PIC...");
    idt_init();
    vga_printf(" OK\n");
    KINFO("IDT loaded, PIC remapped to vectors 32-47\n");

    /* ── Step 4: CPU info ────────────────────────────────────────── */
    vga_printf("  [*] CPU...");
    cpu_init();
    vga_printf(" OK\n");
    cpu_dump();

    const cpu_info_t *ci = cpu_get_info();
    vga_printf("      Vendor: %s  |  %s\n", ci->vendor, ci->brand);

    /* ── Step 5: Physical memory manager ────────────────────────── */
    vga_printf("  [*] PMM...");
    pmm_init(mbi);
    vga_printf(" OK  (%u MB free)\n",
               (pmm_free_page_count() * PAGE_SIZE) / (1024 * 1024));

    /* ── Step 6: Paging ──────────────────────────────────────────── */
    vga_printf("  [*] Paging...");
    paging_init();
    vga_printf(" OK\n");

    /* ── Step 7: Heap ────────────────────────────────────────────── */
    vga_printf("  [*] Heap...");
    heap_init();
    vga_printf(" OK  (at 0x%08x)\n", KHEAP_START);

    /* Quick heap self-test */
    void *p1 = kmalloc(128);
    void *p2 = kzalloc(64);
    if (!p1 || !p2) kpanic("Heap self-test failed!\n");
    kfree(p1);
    kfree(p2);
    KINFO("Heap self-test passed\n");

    /* ── Step 8: PCI ─────────────────────────────────────────────── */
    vga_printf("  [*] PCI...");
    pci_init();
    vga_printf(" OK  (%u device(s))\n", pci_device_count());
    pci_dump();

    /* ── Step 9: Framebuffer (optional) ─────────────────────────── */
    if (fb_init(mbi)) {
        KINFO("Framebuffer: %ux%u @ %ubpp\n",
              mbi->framebuffer_width, mbi->framebuffer_height,
              mbi->framebuffer_bpp);
    }

    /* ── Step 10: Scheduler ──────────────────────────────────────── */
    vga_printf("  [*] Scheduler...");
    sched_init();
    vga_printf(" OK\n");

    /* ── Create demo tasks ───────────────────────────────────────── */
    task_create(demo_task_a, "demo_a");
    task_create(demo_task_b, "demo_b");

    /* ── All done – enable interrupts and drop into the idle loop ── */
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_printf("\n  PinguinOS ready!  Uptime: %u ms\n", sched_uptime_ms());
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    KINFO("All subsystems initialised – enabling interrupts\n");
    sched_dump();

    /* Enable interrupts; the PIT will now drive the scheduler */
    sti();

    /*
     * The boot sequence hands off to the idle task here.
     * In a real kernel we would drop into user space or the shell.
     * For now just spin; the timer IRQ keeps things alive.
     */
    for (;;) {
        hlt();
    }
}
