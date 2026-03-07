/**
 * @file idt.c
 * @brief IDT-Einrichtung, PIC-Remapping und IRQ-Dispatch für PinguinOS.
 *
 * Der Intel 8259A PIC mappt IRQ0–IRQ7 normalerweise auf die Vektoren 8–15,
 * welche mit CPU-Exceptions kollidieren. Wir remappen sie auf die Vektoren 32–47 (IRQ_BASE).
 *
 * Nach dem Remapping:
 *   PIC1 Master: Vektoren 32–39 (IRQ  0–7)
 *   PIC2 Slave:  Vektoren 40–47 (IRQ  8–15)
 */

#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/gdt.h"

/* ── Externe Assembler-Stub-Tabelle (isr.S) ─────────────────────── */
extern void isr0(void);  extern void isr1(void);  extern void isr2(void);
extern void isr3(void);  extern void isr4(void);  extern void isr5(void);
extern void isr6(void);  extern void isr7(void);  extern void isr8(void);
extern void isr9(void);  extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void);
extern void isr15(void); extern void isr16(void); extern void isr17(void);
extern void isr18(void); extern void isr19(void); extern void isr20(void);
extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void);
extern void isr27(void); extern void isr28(void); extern void isr29(void);
extern void isr30(void); extern void isr31(void);
/* IRQ Stubs */
extern void isr32(void); extern void isr33(void); extern void isr34(void);
extern void isr35(void); extern void isr36(void); extern void isr37(void);
extern void isr38(void); extern void isr39(void); extern void isr40(void);
extern void isr41(void); extern void isr42(void); extern void isr43(void);
extern void isr44(void); extern void isr45(void); extern void isr46(void);
extern void isr47(void);

/* ── Externer Assembler-Loader ────────────────────────────────────── */
extern void idt_flush(uint32_t idt_ptr);

/* ── Modul-private Daten ─────────────────────────────────────────── */
static idt_entry_t    idt_entries[IDT_ENTRIES];
static idt_descriptor_t idt_desc;

/* Registrierte C-Ebene Handler */
static irq_handler_t irq_handlers[16];
static irq_handler_t exc_handlers[32];

/* ── Helfer: Ein IDT-Gate kodieren ──────────────────────────────── */
static void idt_set_gate(uint8_t vec, uint32_t handler,
                          uint16_t sel, uint8_t type_attr)
{
    idt_entries[vec].offset_low  = (uint16_t)(handler & 0xFFFF);
    idt_entries[vec].selector    = sel;
    idt_entries[vec].zero        = 0;
    idt_entries[vec].type_attr   = type_attr;
    idt_entries[vec].offset_high = (uint16_t)((handler >> 16) & 0xFFFF);
}

/* ── PIC-Remapping ───────────────────────────────────────────────── */
static void pic_remap(uint8_t offset1, uint8_t offset2)
{
    /* Bestehende Masken speichern */
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    /* ICW1: Initialisierung starten, flankengetriggert, Kaskaden-Modus */
    outb(PIC1_CMD,  0x11); io_wait();
    outb(PIC2_CMD,  0x11); io_wait();

    /* ICW2: Vektor-Offsets */
    outb(PIC1_DATA, offset1); io_wait();
    outb(PIC2_DATA, offset2); io_wait();

    /* ICW3: Master hat Slave auf IR2; Slave ID = 2 */
    outb(PIC1_DATA, 0x04); io_wait();
    outb(PIC2_DATA, 0x02); io_wait();

    /* ICW4: 8086 Modus */
    outb(PIC1_DATA, 0x01); io_wait();
    outb(PIC2_DATA, 0x01); io_wait();

    /* Masken wiederherstellen */
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

/* ── Öffentlich: idt_init ────────────────────────────────────────── */
void idt_init(void)
{
    memset(idt_entries,  0, sizeof(idt_entries));
    memset(irq_handlers, 0, sizeof(irq_handlers));
    memset(exc_handlers, 0, sizeof(exc_handlers));

    /* PIC remappen: IRQ0-7 → Vektor 32, IRQ8-15 → Vektor 40 */
    pic_remap(IRQ_BASE, IRQ_BASE + 8);

    /* Alle IRQs initial maskieren – De-Maskierung bei Treiber-Registrierung */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    /* Exception-Gates installieren (nur Ring 0) */
    uint8_t kgate = IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_GATE_INTERRUPT;

#define SET_EXC(n) idt_set_gate(n, (uint32_t)isr##n, GDT_KERNEL_CODE, kgate)
    SET_EXC(0);  SET_EXC(1);  SET_EXC(2);  SET_EXC(3);
    SET_EXC(4);  SET_EXC(5);  SET_EXC(6);  SET_EXC(7);
    SET_EXC(8);  SET_EXC(9);  SET_EXC(10); SET_EXC(11);
    SET_EXC(12); SET_EXC(13); SET_EXC(14); SET_EXC(15);
    SET_EXC(16); SET_EXC(17); SET_EXC(18); SET_EXC(19);
    SET_EXC(20); SET_EXC(21); SET_EXC(22); SET_EXC(23);
    SET_EXC(24); SET_EXC(25); SET_EXC(26); SET_EXC(27);
    SET_EXC(28); SET_EXC(29); SET_EXC(30); SET_EXC(31);
#undef SET_EXC

    /* IRQ-Gates installieren */
#define SET_IRQ(n) idt_set_gate(n, (uint32_t)isr##n, GDT_KERNEL_CODE, kgate)
    SET_IRQ(32); SET_IRQ(33); SET_IRQ(34); SET_IRQ(35);
    SET_IRQ(36); SET_IRQ(37); SET_IRQ(38); SET_IRQ(39);
    SET_IRQ(40); SET_IRQ(41); SET_IRQ(42); SET_IRQ(43);
    SET_IRQ(44); SET_IRQ(45); SET_IRQ(46); SET_IRQ(47);
#undef SET_IRQ

    idt_desc.size   = (uint16_t)(sizeof(idt_entries) - 1);
    idt_desc.offset = (uint32_t)&idt_entries;
    idt_flush((uint32_t)&idt_desc);
}

/* ── Öffentlich: registrieren / abmelden ────────────────────────── */
void irq_register(uint8_t irq, irq_handler_t handler)
{
    if (irq < 16) {
        irq_handlers[irq] = handler;
        irq_unmask(irq);
    }
}

void irq_unregister(uint8_t irq)
{
    if (irq < 16) {
        irq_handlers[irq] = NULL;
        irq_mask(irq);
    }
}

void exception_register(uint8_t vec, irq_handler_t handler)
{
    if (vec < 32)
        exc_handlers[vec] = handler;
}

/* ── IRQ maskieren / de-maskieren ────────────────────────────────── */
void irq_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t  value;

    if (irq < 8) {
        port  = PIC1_DATA;
    } else {
        port  = PIC2_DATA;
        irq  -= 8;
    }
    value = inb(port) | (uint8_t)(1 << irq);
    outb(port, value);
}

void irq_unmask(uint8_t irq)
{
    uint16_t port;
    uint8_t  value;

    if (irq < 8) {
        port  = PIC1_DATA;
    } else {
        port  = PIC2_DATA;
        irq  -= 8;
    }
    value = inb(port) & (uint8_t)(~(1 << irq));
    outb(port, value);
    /* Cascade-Leitung immer freigeben, wenn ein Slave-IRQ de-maskiert wird */
    if (irq >= 8) {
        value = inb(PIC1_DATA) & ~(1 << IRQ_CASCADE);
        outb(PIC1_DATA, value);
    }
}

/* ── EOI (End of Interrupt) ──────────────────────────────────────── */
void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

/* ── C-Einstiegs-Dispatcher (aufgerufen von isr_common_stub) ────── */
static const char *exception_names[] = {
    "Divide Error",        "Debug",              "NMI",
    "Breakpoint",          "Overflow",           "BOUND Range Exceeded",
    "Invalid Opcode",      "Device Not Available","Double Fault",
    "Coproc Seg Overrun",  "Invalid TSS",        "Segment Not Present",
    "Stack Fault",         "General Protection", "Page Fault",
    "Reserved",            "x87 FP Exception",   "Alignment Check",
    "Machine Check",       "SIMD FP Exception",  "Virtualisation",
    "Control Protection",  "Reserved",           "Reserved",
    "Reserved",            "Reserved",           "Reserved",
    "Reserved",            "Reserved",           "Reserved",
    "Security Exception",  "Reserved"
};

void isr_dispatch(cpu_regs_t *regs)
{
    if (regs->int_no < 32) {
        /* CPU Exception */
        if (exc_handlers[regs->int_no]) {
            exc_handlers[regs->int_no](regs);
        } else {
            /* Nicht behandelte Exception → Kernel Panic */
            kpanic("Unbehandelte CPU Exception #%u (%s)\n"
                   "  err_code=0x%08x  eip=0x%08x  cs=0x%04x\n"
                   "  eax=0x%08x  ebx=0x%08x  ecx=0x%08x  edx=0x%08x\n"
                   "  esi=0x%08x  edi=0x%08x  ebp=0x%08x  esp=0x%08x\n",
                   regs->int_no,
                   (regs->int_no < 32) ? exception_names[regs->int_no] : "?",
                   regs->err_code, regs->eip, regs->cs,
                   regs->eax, regs->ebx, regs->ecx, regs->edx,
                   regs->esi, regs->edi, regs->ebp, regs->esp_dummy);
        }
    } else if (regs->int_no < 48) {
        /* Hardware IRQ */
        uint8_t irq = (uint8_t)(regs->int_no - IRQ_BASE);

        if (irq_handlers[irq])
            irq_handlers[irq](regs);

        pic_send_eoi(irq);
    }
}
