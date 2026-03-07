/*
 * boot.S  –  PinguinOS x86 Multiboot-Bootstrap
 *
 * Dies ist der allererste Code, den die CPU ausführt, nachdem GRUB den
 * Kernel geladen hat. Er erledigt:
 *   1. Einbetten eines Multiboot-1-Headers, damit GRUB das Image erkennt.
 *   2. Einrichten eines kleinen initialen Kernel-Stacks.
 *   3. Speichern des Multiboot-Magic-Wertes und des Pointers auf die Info-Struktur.
 *   4. Aufruf des C-Einstiegspunkts cmain(magic, mbi_ptr).
 *   5. Anhalten in einer Endlosschleife, falls cmain jemals zurückkehrt (sollte nicht passieren).
 *
 * Build:  i686-elf-gcc -m32 -c boot.S -o boot.o
 */

    .set MULTIBOOT_MAGIC,    0x1BADB002
    .set MULTIBOOT_FLAGS,    0x00000003   /* Bit 0: Page-Align, Bit 1: Memory-Map */
    .set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

    /* Stack-Größe: 16 KB, 16-Byte-Alignment gemäß SysV-ABI-Anforderungen */
    .set STACK_SIZE, 0x4000

/* ── Multiboot-Header (muss in den ersten 8 KB des Images liegen) ── */
    .section .multiboot, "a"
    .align 4
multiboot_header:
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM

/* ── Initialer Stack (in BSS, um das Binary nicht unnötig aufzublähen) ── */
    .section .bss, "aw", @nobits
    .align 16
stack_bottom:
    .skip STACK_SIZE
stack_top:

/* ── Einstiegspunkt ─────────────────────────────────────────────── */
    .section .text
    .global _start
    .type   _start, @function
_start:
    /*
     * Zu diesem Zeitpunkt befindet sich die CPU im 32-Bit Protected Mode.
     * EAX = MULTIBOOT_MAGIC  (0x2BADB002)
     * EBX = physische Adresse der multiboot_info_t Struktur
     * Interrupts sind deaktiviert; noch keine gültigen GDT/IDT vorhanden.
     */

    /* Den Stack-Pointer auf das obere Ende unseres BSS-Stacks setzen */
    mov $stack_top, %esp

    /* Den Stack auf 16 Byte ausrichten, wie von der SysV i386 ABI gefordert */
    andl $-16, %esp

    /*
     * Argumente für cmain(uint32_t magic, multiboot_info_t *mbi) pushen.
     * Argumente werden gemäß C-Aufrufkonvention von rechts nach links gepusht.
     */
    pushl %ebx    /* arg2: multiboot_info_t* */
    pushl %eax    /* arg1: magic             */

    /* Den C-Kernel-Einstiegspunkt aufrufen */
    call  cmain

    /*
     * cmain sollte niemals zurückkehren. Falls doch, Interrupts deaktivieren
     * und die CPU in einer Endlosschleife anhalten.
     */
    cli
.Lhang:
    hlt
    jmp .Lhang

    .size _start, . - _start

/* ── Segmentregister nach GDT-Reload flushen ─────────────────────── */
    /*
     * gdt_flush(uint32_t gdt_ptr)
     *
     * Wird von C aufgerufen, um den Far-Jump auszuführen, der CS mit dem
     * neuen Kernel-Code-Segment-Selektor (0x08) lädt und anschließend
     * die Datensegment-Register neu lädt.
     *
     * Prototyp: extern void gdt_flush(uint32_t gdt_ptr);
     */
    .global gdt_flush
    .type   gdt_flush, @function
gdt_flush:
    movl  4(%esp), %eax     /* EAX = Pointer auf gdt_descriptor_t    */
    lgdt  (%eax)            /* Die neue GDT laden                    */

    /* Far-Jump, um CS mit dem Kernel-Code-Selektor (0x08) neu zu laden */
    ljmp  $0x08, $1f
1:
    /* Alle Datensegment-Register mit dem Kernel-Data-Selektor neu laden */
    movw  $0x10, %ax
    movw  %ax, %ds
    movw  %ax, %es
    movw  %ax, %fs
    movw  %ax, %gs
    movw  %ax, %ss
    ret

    .size gdt_flush, . - gdt_flush

/* ── TSS laden ───────────────────────────────────────────────────── */
    /*
     * tss_flush()
     *
     * Lädt den TSS-Selektor (0x28 | 0x03 RPL=3 → eigentlich 0x2B für
     * ein User-sichtbares TSS, aber wir nutzen 0x28, da wir es nur
     * für den Ring-0-Stack bei Interrupts benötigen).
     *
     * Prototyp: extern void tss_flush(void);
     */
    .global tss_flush
    .type   tss_flush, @function
tss_flush:
    movw  $0x28, %ax        /* TSS-Selektor                          */
    ltr   %ax
    ret

    .size tss_flush, . - tss_flush

/* ── IDT laden ───────────────────────────────────────────────────── */
    /*
     * idt_flush(uint32_t idt_ptr)
     *
     * Prototyp: extern void idt_flush(uint32_t idt_ptr);
     */
    .global idt_flush
    .type   idt_flush, @function
idt_flush:
    movl  4(%esp), %eax
    lidt  (%eax)
    ret

    .size idt_flush, . - idt_flush

/* ── Kontextwechsel (Context Switch) ─────────────────────────────── */
    /*
     * sched_switch_context(task_context_t *old, task_context_t *new)
     *
     * Speichert die aktuellen CPU-Register in *old und stellt sie aus *new wieder her.
     * Prototyp: extern void sched_switch_context(task_context_t *old,
     *                                              task_context_t *new);
     *
     * task_context_t Layout (muss mit scheduler.h übereinstimmen):
     *   +0   edi
     *   +4   esi
     *   +8   ebp
     *   +12  esp
     *   +16  ebx
     *   +20  edx
     *   +24  ecx
     *   +28  eax
     *   +32  eip   (Rücksprungadresse)
     *   +36  eflags
     */
    .global sched_switch_context
    .type   sched_switch_context, @function
sched_switch_context:
    movl  4(%esp), %eax     /* EAX = Pointer auf alten Kontext       */
    movl  8(%esp), %ecx     /* ECX = Pointer auf neuen Kontext       */

    /* Rechenregister in *old speichern */
    movl  %edi,  0(%eax)
    movl  %esi,  4(%eax)
    movl  %ebp,  8(%eax)
    movl  %esp, 12(%eax)
    movl  %ebx, 16(%eax)
    movl  %edx, 20(%eax)
    /* ECX wird bereits als Scratch-Register genutzt; ursprüngliches ECX speichern */
    movl  %ecx, 24(%eax)    /* wird nach der Wiederherstellung überschrieben */
    movl  %eax, 28(%eax)    /* EAX speichern (alter Kontext-Pointer, nur Platzhalter) */

    /* Rücksprungadresse (EIP) und EFLAGS speichern */
    movl  (%esp), %edx      /* Rücksprungadresse, die durch CALL gepusht wurde */
    movl  %edx,  32(%eax)
    pushf
    popl  %edx
    movl  %edx,  36(%eax)

    /* Wiederherstellen aus *new */
    movl  0(%ecx), %edi
    movl  4(%ecx), %esi
    movl  8(%ecx), %ebp
    movl 12(%ecx), %esp
    movl 16(%ecx), %ebx
    movl 20(%ecx), %edx
    /* EFLAGS wiederherstellen */
    pushl 36(%ecx)
    popf
    /* EAX und ECX als Letztes wiederherstellen */
    movl 28(%ecx), %eax
    movl 24(%ecx), %ecx

    /* Zurückspringen zur gespeicherten EIP des neuen Tasks (liegt bereits auf dessen Stack) */
    ret

    .size sched_switch_context, . - sched_switch_context
