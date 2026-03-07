/*
 * boot.S  –  PinguinOS x86 Multiboot bootstrap
 *
 * This is the very first code the CPU executes after GRUB loads the
 * kernel.  It:
 *   1. Embeds a Multiboot 1 header so GRUB can identify the image.
 *   2. Sets up a small initial kernel stack.
 *   3. Saves the Multiboot magic value and info-struct pointer.
 *   4. Calls the C entry point  cmain(magic, mbi_ptr).
 *   5. Halts in a loop if cmain ever returns (it should not).
 *
 * Build:  i686-elf-gcc -m32 -c boot.S -o boot.o
 */

    .set MULTIBOOT_MAGIC,    0x1BADB002
    .set MULTIBOOT_FLAGS,    0x00000003   /* Bit 0: page-align, Bit 1: mem map */
    .set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

    /* Stack size: 16 KB, 16-byte aligned as required by the SysV ABI */
    .set STACK_SIZE, 0x4000

/* ── Multiboot header (must be in the first 8 KB of the image) ──── */
    .section .multiboot, "a"
    .align 4
multiboot_header:
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM

/* ── Initial stack (in BSS so it doesn't bloat the binary) ─────── */
    .section .bss, "aw", @nobits
    .align 16
stack_bottom:
    .skip STACK_SIZE
stack_top:

/* ── Entry point ─────────────────────────────────────────────────── */
    .section .text
    .global _start
    .type   _start, @function
_start:
    /*
     * At this point the CPU is in 32-bit protected mode.
     * EAX = MULTIBOOT_MAGIC  (0x2BADB002)
     * EBX = physical address of the multiboot_info_t structure
     * Interrupts are disabled; no valid GDT/IDT yet.
     */

    /* Point the stack register to the top of our BSS stack */
    mov $stack_top, %esp

    /* Align the stack to 16 bytes as required by the SysV i386 ABI */
    andl $-16, %esp

    /*
     * Push arguments for cmain(uint32_t magic, multiboot_info_t *mbi).
     * Arguments are pushed right-to-left in the C calling convention.
     */
    pushl %ebx    /* arg2: multiboot_info_t* */
    pushl %eax    /* arg1: magic             */

    /* Call the C kernel entry point */
    call  cmain

    /*
     * cmain should never return.  If it does, disable interrupts and
     * halt the CPU in an infinite loop.
     */
    cli
.Lhang:
    hlt
    jmp .Lhang

    .size _start, . - _start

/* ── Flush segment registers after GDT reload ───────────────────── */
    /*
     * gdt_flush(uint32_t gdt_ptr)
     *
     * Called from C to perform the far jump that reloads CS with the
     * new kernel code segment selector (0x08) and then reloads the
     * data segment registers.
     *
     * Prototype: extern void gdt_flush(uint32_t gdt_ptr);
     */
    .global gdt_flush
    .type   gdt_flush, @function
gdt_flush:
    movl  4(%esp), %eax     /* EAX = pointer to gdt_descriptor_t    */
    lgdt  (%eax)            /* Load the new GDT                      */

    /* Far jump to reload CS with the kernel code selector (0x08)   */
    ljmp  $0x08, $1f
1:
    /* Reload all data segment registers with the kernel data selector */
    movw  $0x10, %ax
    movw  %ax, %ds
    movw  %ax, %es
    movw  %ax, %fs
    movw  %ax, %gs
    movw  %ax, %ss
    ret

    .size gdt_flush, . - gdt_flush

/* ── Load the TSS ────────────────────────────────────────────────── */
    /*
     * tss_flush()
     *
     * Loads the TSS selector (0x28 | 0x03 RPL=3 → actually 0x2B for
     * user-visible TSS, but we use 0x28 since we only need it for
     * ring-0 stack on interrupts).
     *
     * Prototype: extern void tss_flush(void);
     */
    .global tss_flush
    .type   tss_flush, @function
tss_flush:
    movw  $0x28, %ax        /* TSS selector                          */
    ltr   %ax
    ret

    .size tss_flush, . - tss_flush

/* ── Load the IDT ────────────────────────────────────────────────── */
    /*
     * idt_flush(uint32_t idt_ptr)
     *
     * Prototype: extern void idt_flush(uint32_t idt_ptr);
     */
    .global idt_flush
    .type   idt_flush, @function
idt_flush:
    movl  4(%esp), %eax
    lidt  (%eax)
    ret

    .size idt_flush, . - idt_flush

/* ── Context switch ──────────────────────────────────────────────── */
    /*
     * sched_switch_context(task_context_t *old, task_context_t *new)
     *
     * Saves the current CPU registers into *old and restores from *new.
     * Prototype: extern void sched_switch_context(task_context_t *old,
     *                                              task_context_t *new);
     *
     * task_context_t layout (must match scheduler.h):
     *   +0   edi
     *   +4   esi
     *   +8   ebp
     *   +12  esp
     *   +16  ebx
     *   +20  edx
     *   +24  ecx
     *   +28  eax
     *   +32  eip   (return address)
     *   +36  eflags
     */
    .global sched_switch_context
    .type   sched_switch_context, @function
sched_switch_context:
    movl  4(%esp), %eax     /* EAX = old context pointer             */
    movl  8(%esp), %ecx     /* ECX = new context pointer             */

    /* Save general-purpose registers into *old */
    movl  %edi,  0(%eax)
    movl  %esi,  4(%eax)
    movl  %ebp,  8(%eax)
    movl  %esp, 12(%eax)
    movl  %ebx, 16(%eax)
    movl  %edx, 20(%eax)
    /* ECX already used as scratch; save original ECX */
    movl  %ecx, 24(%eax)    /* will be overwritten after restore     */
    movl  %eax, 28(%eax)    /* save EAX (old ctx ptr, just a placeholder) */

    /* Save return address (EIP) and EFLAGS */
    movl  (%esp), %edx      /* return address pushed by CALL         */
    movl  %edx,  32(%eax)
    pushf
    popl  %edx
    movl  %edx,  36(%eax)

    /* Restore from *new */
    movl  0(%ecx), %edi
    movl  4(%ecx), %esi
    movl  8(%ecx), %ebp
    movl 12(%ecx), %esp
    movl 16(%ecx), %ebx
    movl 20(%ecx), %edx
    /* Restore EFLAGS */
    pushl 36(%ecx)
    popf
    /* Restore EAX and ECX last */
    movl 28(%ecx), %eax
    movl 24(%ecx), %ecx

    /* Return to the new task's saved EIP (already on its stack) */
    ret

    .size sched_switch_context, . - sched_switch_context
