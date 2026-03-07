/*
 * isr.S  –  PinguinOS interrupt / exception stubs
 *
 * For every IDT vector we generate a small stub that:
 *   1. Optionally pushes a dummy error code (vectors without a CPU-pushed
 *      error code get a 0 pushed to keep the stack frame uniform).
 *   2. Pushes the interrupt number.
 *   3. Jumps to the common handler  isr_common_stub.
 *
 * isr_common_stub saves all registers, calls the C handler
 * isr_dispatch(cpu_regs_t*), then restores and returns.
 */

/* ── Macros ──────────────────────────────────────────────────────── */

/*
 * ISR_NOERR(n)  – exception without a CPU-supplied error code.
 * Push a dummy 0 so every frame looks identical to the C handler.
 */
.macro ISR_NOERR n
    .global isr\n
    isr\n:
        pushl $0          /* dummy error code */
        pushl $\n         /* interrupt number */
        jmp   isr_common_stub
.endm

/*
 * ISR_ERR(n)  – exception WITH a CPU-supplied error code.
 * The CPU already pushed the error code before we get here.
 */
.macro ISR_ERR n
    .global isr\n
    isr\n:
        pushl $\n         /* interrupt number (error code already on stack) */
        jmp   isr_common_stub
.endm

/* ── CPU exception stubs (vectors 0–31) ─────────────────────────── */
ISR_NOERR  0    /* #DE Divide Error                             */
ISR_NOERR  1    /* #DB Debug                                    */
ISR_NOERR  2    /* NMI Non-Maskable Interrupt                   */
ISR_NOERR  3    /* #BP Breakpoint                               */
ISR_NOERR  4    /* #OF Overflow                                 */
ISR_NOERR  5    /* #BR BOUND Range Exceeded                     */
ISR_NOERR  6    /* #UD Invalid Opcode                           */
ISR_NOERR  7    /* #NM Device Not Available (FPU)               */
ISR_ERR    8    /* #DF Double Fault          (error code = 0)   */
ISR_NOERR  9    /* Coprocessor Segment Overrun (obsolete)       */
ISR_ERR   10    /* #TS Invalid TSS                              */
ISR_ERR   11    /* #NP Segment Not Present                      */
ISR_ERR   12    /* #SS Stack-Segment Fault                      */
ISR_ERR   13    /* #GP General Protection Fault                 */
ISR_ERR   14    /* #PF Page Fault                               */
ISR_NOERR 15    /* Reserved                                     */
ISR_NOERR 16    /* #MF x87 Floating-Point Exception             */
ISR_ERR   17    /* #AC Alignment Check                          */
ISR_NOERR 18    /* #MC Machine Check                            */
ISR_NOERR 19    /* #XM/#XF SIMD Floating-Point Exception        */
ISR_NOERR 20    /* #VE Virtualisation Exception                 */
ISR_ERR   21    /* #CP Control-Protection Exception             */
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30    /* #SX Security Exception                       */
ISR_NOERR 31

/* ── Hardware IRQ stubs (vectors 32–47) ─────────────────────────── */
ISR_NOERR 32    /* IRQ0  – PIT timer                            */
ISR_NOERR 33    /* IRQ1  – PS/2 keyboard                        */
ISR_NOERR 34    /* IRQ2  – cascade (PIC2)                       */
ISR_NOERR 35    /* IRQ3  – COM2                                 */
ISR_NOERR 36    /* IRQ4  – COM1                                 */
ISR_NOERR 37    /* IRQ5  – LPT2 / Sound                         */
ISR_NOERR 38    /* IRQ6  – Floppy disk controller               */
ISR_NOERR 39    /* IRQ7  – LPT1 / Spurious                      */
ISR_NOERR 40    /* IRQ8  – RTC                                  */
ISR_NOERR 41    /* IRQ9  – ACPI                                 */
ISR_NOERR 42    /* IRQ10 – free                                 */
ISR_NOERR 43    /* IRQ11 – free                                 */
ISR_NOERR 44    /* IRQ12 – PS/2 mouse                           */
ISR_NOERR 45    /* IRQ13 – FPU / co-processor                   */
ISR_NOERR 46    /* IRQ14 – ATA primary                          */
ISR_NOERR 47    /* IRQ15 – ATA secondary                        */

/* ── Common handler stub ─────────────────────────────────────────── */
    .extern isr_dispatch

    .type isr_common_stub, @function
isr_common_stub:
    /*
     * At entry the stack looks like (top → bottom):
     *   [int_no]   ← pushed by macro
     *   [err_code] ← pushed by macro or CPU
     *   [eip]      ← pushed by CPU (interrupt return address)
     *   [cs]
     *   [eflags]
     *   ([esp])    ← only on privilege-level change
     *   ([ss])
     */

    /* Save general-purpose registers (pushad order: eax ecx edx ebx esp ebp esi edi) */
    pusha

    /* Save segment registers */
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    /* Load the kernel data segment selector into all data segments */
    movw  $0x10, %ax
    movw  %ax, %ds
    movw  %ax, %es
    movw  %ax, %fs
    movw  %ax, %gs

    /* Call the C dispatcher with a pointer to the register frame */
    pushl %esp              /* arg: cpu_regs_t* regs              */
    call  isr_dispatch
    addl  $4, %esp

    /* Restore segment registers */
    popl  %gs
    popl  %fs
    popl  %es
    popl  %ds

    /* Restore general-purpose registers */
    popa

    /* Remove the int_no and err_code pushed by the macro/CPU */
    addl  $8, %esp

    /* Return from interrupt (restores EIP, CS, EFLAGS [, ESP, SS]) */
    iret

    .size isr_common_stub, . - isr_common_stub
