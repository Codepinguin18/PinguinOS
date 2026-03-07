/*
 * isr.S  –  PinguinOS Interrupt- / Exception-Stubs
 *
 * Für jeden IDT-Vektor generieren wir einen kleinen Stub, der:
 *   1. Optional einen Dummy-Fehlercode pusht (Vektoren ohne CPU-generierten
 *      Fehlercode erhalten eine 0, um den Stack-Frame einheitlich zu halten).
 *   2. Die Interrupt-Nummer pusht.
 *   3. Zum gemeinsamen Handler isr_common_stub springt.
 *
 * isr_common_stub sichert alle Register, ruft den C-Handler
 * isr_dispatch(cpu_regs_t*) auf, stellt anschließend alles wieder her und kehrt zurück.
 */

/* ── Makros ──────────────────────────────────────────────────────── */

/*
 * ISR_NOERR(n) – Exception ohne CPU-seitigen Fehlercode.
 * Pusht eine Dummy-0, damit jeder Frame für den C-Handler identisch aussieht.
 */
.macro ISR_NOERR n
    .global isr\n
    isr\n:
        pushl $0          /* Dummy-Fehlercode */
        pushl $\n         /* Interrupt-Nummer */
        jmp   isr_common_stub
.endm

/*
 * ISR_ERR(n) – Exception MIT CPU-seitigem Fehlercode.
 * Die CPU hat den Fehlercode bereits gepusht, bevor wir hier ankommen.
 */
.macro ISR_ERR n
    .global isr\n
    isr\n:
        pushl $\n         /* Interrupt-Nummer (Fehlercode liegt bereits auf dem Stack) */
        jmp   isr_common_stub
.endm

/* ── CPU-Exception-Stubs (Vektoren 0–31) ────────────────────────── */
ISR_NOERR  0    /* #DE Divide Error (Divisionsfehler)           */
ISR_NOERR  1    /* #DB Debug                                    */
ISR_NOERR  2    /* NMI Non-Maskable Interrupt                   */
ISR_NOERR  3    /* #BP Breakpoint                               */
ISR_NOERR  4    /* #OF Overflow (Überlauf)                       */
ISR_NOERR  5    /* #BR BOUND Range Exceeded                     */
ISR_NOERR  6    /* #UD Invalid Opcode (Ungültiger Opcode)        */
ISR_NOERR  7    /* #NM Device Not Available (FPU nicht verfügbar)*/
ISR_ERR    8    /* #DF Double Fault          (Fehlercode = 0)   */
ISR_NOERR  9    /* Coprocessor Segment Overrun (veraltet)       */
ISR_ERR   10    /* #TS Invalid TSS                              */
ISR_ERR   11    /* #NP Segment Not Present                      */
ISR_ERR   12    /* #SS Stack-Segment Fault                      */
ISR_ERR   13    /* #GP General Protection Fault (Allg. Schutz)  */
ISR_ERR   14    /* #PF Page Fault (Seitenfehler)                 */
ISR_NOERR 15    /* Reserviert                                   */
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

/* ── Hardware-IRQ-Stubs (Vektoren 32–47) ────────────────────────── */
ISR_NOERR 32    /* IRQ0  – PIT Timer                            */
ISR_NOERR 33    /* IRQ1  – PS/2 Tastatur                        */
ISR_NOERR 34    /* IRQ2  – Kaskade (PIC2)                       */
ISR_NOERR 35    /* IRQ3  – COM2                                 */
ISR_NOERR 36    /* IRQ4  – COM1                                 */
ISR_NOERR 37    /* IRQ5  – LPT2 / Sound                         */
ISR_NOERR 38    /* IRQ6  – Floppy-Disk-Controller               */
ISR_NOERR 39    /* IRQ7  – LPT1 / Spurious                      */
ISR_NOERR 40    /* IRQ8  – RTC (Echtzeituhr)                    */
ISR_NOERR 41    /* IRQ9  – ACPI                                 */
ISR_NOERR 42    /* IRQ10 – frei                                 */
ISR_NOERR 43    /* IRQ11 – frei                                 */
ISR_NOERR 44    /* IRQ12 – PS/2 Maus                            */
ISR_NOERR 45    /* IRQ13 – FPU / Co-Prozessor                   */
ISR_NOERR 46    /* IRQ14 – ATA Primär                           */
ISR_NOERR 47    /* IRQ15 – ATA Sekundär                         */

/* ── Gemeinsamer Handler-Stub (isr_common_stub) ─────────────────── */
    .extern isr_dispatch

    .type isr_common_stub, @function
isr_common_stub:
    /*
     * Bei Einstieg sieht der Stack wie folgt aus (oben → unten):
     *   [int_no]   ← durch Makro gepusht
     *   [err_code] ← durch Makro oder CPU gepusht
     *   [eip]      ← durch CPU gepusht (Interrupt-Rücksprungadresse)
     *   [cs]
     *   [eflags]
     *   ([esp])    ← nur bei Privilegienstufen-Wechsel
     *   ([ss])
     */

    /* Allgemeine Register sichern (pushad Reihenfolge: eax ecx edx ebx esp ebp esi edi) */
    pusha

    /* Segmentregister sichern */
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    /* Kernel-Datensegment-Selektor in alle Datensegmente laden */
    movw  $0x10, %ax
    movw  %ax, %ds
    movw  %ax, %es
    movw  %ax, %fs
    movw  %ax, %gs

    /* Den C-Dispatcher mit einem Pointer auf den Register-Frame aufrufen */
    pushl %esp              /* arg: cpu_regs_t* regs              */
    call  isr_dispatch
    addl  $4, %esp

    /* Segmentregister wiederherstellen */
    popl  %gs
    popl  %fs
    popl  %es
    popl  %ds

    /* Allgemeine Register wiederherstellen */
    popa

    /* int_no und err_code entfernen (vom Makro/CPU gepusht) */
    addl  $8, %esp

    /* Rückkehr aus dem Interrupt (stellt EIP, CS, EFLAGS [, ESP, SS] wieder her) */
    iret

    .size isr_common_stub, . - isr_common_stub
