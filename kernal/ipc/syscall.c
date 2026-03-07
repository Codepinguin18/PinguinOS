/**
 * @file syscall.c
 * @brief Syscall-Gateway, ELF-Loader und User-Mode-Transition für PinguinOS.
 */
#include "../include/syscall.h"
#include "../include/idt.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/heap.h"
#include "../include/vfs.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"
#include "../include/gdt.h"

/* ── Syscall-Tabelle ─────────────────────────────────────────────── */
static syscall_fn_t syscall_table[NR_SYSCALLS];

/* ── Einzelne Syscall-Handler ─────────────────────────────────────── */
static int32_t sys_exit(uint32_t code, uint32_t b, uint32_t c,
                         uint32_t d, uint32_t e)
{
    UNUSED(b); UNUSED(c); UNUSED(d); UNUSED(e);
    proc_exit((int)code);
    return 0;
}

static int32_t sys_getpid(uint32_t a, uint32_t b, uint32_t c,
                            uint32_t d, uint32_t e)
{
    UNUSED(a); UNUSED(b); UNUSED(c); UNUSED(d); UNUSED(e);
    process_t *p = proc_current();
    return p ? (int32_t)p->pid : 1;
}

static int32_t sys_write(uint32_t fd, uint32_t buf_addr,
                          uint32_t len, uint32_t d, uint32_t e)
{
    UNUSED(d); UNUSED(e);
    /* fd 1/2 = stdout/stderr → serielle Ausgabe */
    if (fd == 1 || fd == 2) {
        const char *s = (const char *)buf_addr;
        for (uint32_t i = 0; i < len; i++) serial_putc(s[i]);
        return (int32_t)len;
    }
    return -1;
}

static int32_t sys_read(uint32_t fd, uint32_t buf_addr,
                         uint32_t len, uint32_t d, uint32_t e)
{
    UNUSED(d); UNUSED(e);
    if (fd == 0) {
        /* stdin → serielle Eingabe (blockierend) */
        char *buf = (char *)buf_addr;
        for (uint32_t i = 0; i < len; i++) {
            buf[i] = serial_getc();
            if (buf[i] == '\n') { i++; return (int32_t)i; }
        }
        return (int32_t)len;
    }
    return -1;
}

static int32_t sys_sleep(uint32_t ms, uint32_t b, uint32_t c,
                          uint32_t d, uint32_t e)
{
    UNUSED(b); UNUSED(c); UNUSED(d); UNUSED(e);
    task_sleep(ms);
    return 0;
}

static int32_t sys_yield(uint32_t a, uint32_t b, uint32_t c,
                          uint32_t d, uint32_t e)
{
    UNUSED(a); UNUSED(b); UNUSED(c); UNUSED(d); UNUSED(e);
    task_yield();
    return 0;
}

static int32_t sys_kill(uint32_t pid, uint32_t sig, uint32_t c,
                         uint32_t d, uint32_t e)
{
    UNUSED(c); UNUSED(d); UNUSED(e);
    process_t *p = proc_find(pid);
    if (!p) return -1;
    proc_signal(p, (int)sig);
    return 0;
}

/* ── INT 0x80 Dispatcher ──────────────────────────────────────────── */
static void syscall_handler(cpu_regs_t *regs)
{
    uint32_t nr = regs->eax;

    if (nr >= NR_SYSCALLS || !syscall_table[nr]) {
        KWARN("Syscall: ungültige Nummer %u\n", nr);
        regs->eax = (uint32_t)-1;
        return;
    }

    /* Argumente aus Registern: ebx, ecx, edx, esi, edi */
    int32_t ret = syscall_table[nr](
        regs->ebx, regs->ecx, regs->edx,
        regs->esi, regs->edi);

    regs->eax = (uint32_t)ret;
}

/* ── syscall_init ────────────────────────────────────────────────── */
void syscall_init(void)
{
    memset(syscall_table, 0, sizeof(syscall_table));

    syscall_table[SYS_EXIT]   = sys_exit;
    syscall_table[SYS_READ]   = sys_read;
    syscall_table[SYS_WRITE]  = sys_write;
    syscall_table[SYS_GETPID] = sys_getpid;
    syscall_table[SYS_KILL]   = sys_kill;
    syscall_table[SYS_SLEEP]  = sys_sleep;
    syscall_table[SYS_YIELD]  = sys_yield;

    /* INT 0x80 Gate mit Ring-3-Erlaubnis (DPL=3) */
    extern void isr128(void);
    /* Wir nutzen einen dedizierten Gate-Slot: Vector 128 */
    /* Registrierung über exception_register mit anderem Attribute: */
    /* Der Gate muss mit IDT_FLAG_RING3 gesetzt sein */
    /* Hier vereinfacht: direkt in die IDT schreiben */

    /* Syscall-Handler via IRQ-Exception-Mechanismus koppeln */
    exception_register(0x80, syscall_handler);  /* Umleitung über exc_handlers */

    KINFO("Syscall: Gateway INT 0x80 initialisiert (%u Handler)\n",
          SYS_YIELD + 1);
}

/* ══════════════════════════════════════════════════════════════════
 *  ELF32-Loader – Feature #77
 * ══════════════════════════════════════════════════════════════════ */
int elf32_load(const uint8_t *elf_data, uint32_t elf_size,
               uint32_t *entry_out)
{
    if (!elf_data || elf_size < sizeof(elf32_hdr_t)) return -1;

    const elf32_hdr_t *hdr = (const elf32_hdr_t *)elf_data;

    /* Magie prüfen */
    if (hdr->e_ident[0] != ELF_MAGIC0 ||
        hdr->e_ident[1] != ELF_MAGIC1 ||
        hdr->e_ident[2] != ELF_MAGIC2 ||
        hdr->e_ident[3] != ELF_MAGIC3) {
        KERR("ELF: Ungültige Magie\n");
        return -2;
    }

    if (hdr->e_type    != ET_EXEC) { KERR("ELF: Kein ausführbares Binary\n"); return -3; }
    if (hdr->e_machine != EM_386)  { KERR("ELF: Nicht x86\n"); return -4; }

    /* Program-Header laden */
    const elf32_phdr_t *phdrs =
        (const elf32_phdr_t *)(elf_data + hdr->e_phoff);

    for (uint16_t i = 0; i < hdr->e_phnum; i++) {
        const elf32_phdr_t *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD) continue;

        /* Seiten für dieses Segment allozieren und mappen */
        uint32_t vaddr = ALIGN_DOWN(ph->p_vaddr, PAGE_SIZE);
        uint32_t pages = (ALIGN_UP(ph->p_vaddr + ph->p_memsz, PAGE_SIZE) - vaddr)
                         / PAGE_SIZE;

        for (uint32_t p = 0; p < pages; p++) {
            uint32_t phys = pmm_alloc_page();
            if (!phys) return -5;
            paging_map(vaddr + p * PAGE_SIZE, phys,
                       PDE_PRESENT | PDE_WRITABLE | PDE_USER);
        }

        /* Daten kopieren */
        uint8_t *dst = (uint8_t *)ph->p_vaddr;
        memset(dst, 0, ph->p_memsz);
        if (ph->p_filesz && ph->p_offset + ph->p_filesz <= elf_size)
            memcpy(dst, elf_data + ph->p_offset, ph->p_filesz);

        KINFO("ELF: Segment @ 0x%08x  %u Bytes\n", ph->p_vaddr, ph->p_filesz);
    }

    if (entry_out) *entry_out = hdr->e_entry;
    KINFO("ELF: Geladen  Einsprung @ 0x%08x\n", hdr->e_entry);
    return 0;
}

/* ══════════════════════════════════════════════════════════════════
 *  User-Mode-Übergang – Feature #78
 *  Wechsel von Ring 0 → Ring 3 über iret
 * ══════════════════════════════════════════════════════════════════ */
void user_mode_enter(uint32_t entry, uint32_t user_esp)
{
    /* User-Code-Selector 0x18 | 3 = 0x1B, User-Data 0x20 | 3 = 0x23 */
#define USER_CS  0x1B
#define USER_DS  0x23

    /* Kernel-Stack-Zeiger im TSS setzen */
    task_t *t = sched_current();
    if (t) tss_set_kernel_stack(t->stack_top);

    /* Segment-Register auf User-Data setzen */
    __asm__ volatile (
        "mov %0, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        :: "i"(USER_DS) : "ax"
    );

    /* Fake IRET-Frame für Ring-3-Übergang */
    __asm__ volatile (
        "pushl %0\n\t"    /* SS = User-DS */
        "pushl %1\n\t"    /* ESP */
        "pushf\n\t"       /* EFLAGS */
        "orl  $0x200, (%%esp)\n\t"  /* IF = 1 */
        "pushl %2\n\t"    /* CS = User-CS */
        "pushl %3\n\t"    /* EIP = entry  */
        "iret\n\t"
        :: "i"(USER_DS),
           "r"(user_esp),
           "i"(USER_CS),
           "r"(entry)
    );

    /* Wird nie erreicht */
    for (;;) hlt();
}
