/**
 * linker.ld  –  PinguinOS kernel linker script (x86 32-bit)
 *
 * Virtual load address: 1 MB (0x00100000) – standard for Multiboot kernels.
 *
 * Sections:
 *   .multiboot  – Multiboot header (must appear in first 8 KB)
 *   .text       – Executable code
 *   .rodata     – Read-only data (const strings, tables)
 *   .data       – Initialised writable data
 *   .bss        – Uninitialised writable data (zero-filled by loader)
 *
 * The linker exports two symbols used by the physical memory manager:
 *   _kernel_start  – first byte of the kernel image (physical)
 *   _kernel_end    – first byte after the kernel image (physical)
 *                    rounded up to a 4 KB page boundary
 */

ENTRY(_start)

SECTIONS
{
    /* ── Load address ────────────────────────────────────────────── */
    . = 0x00100000;

    _kernel_start = .;

    /* ── Multiboot header (first 8 KB) ──────────────────────────── */
    .multiboot ALIGN(4) :
    {
        *(.multiboot)
    }

    /* ── Code ────────────────────────────────────────────────────── */
    .text ALIGN(4096) :
    {
        *(.text .text.*)
    }

    /* ── Read-only data ──────────────────────────────────────────── */
    .rodata ALIGN(4096) :
    {
        *(.rodata .rodata.*)
    }

    /* ── Initialised data ────────────────────────────────────────── */
    .data ALIGN(4096) :
    {
        *(.data .data.*)
    }

    /* ── Uninitialised data (BSS) ────────────────────────────────── */
    .bss ALIGN(4096) :
    {
        *(COMMON)
        *(.bss .bss.*)
    }

    /* ── End marker (page-aligned) ───────────────────────────────── */
    . = ALIGN(4096);
    _kernel_end = .;

    /* ── Discard debug and other sections ───────────────────────── */
    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
        *(.eh_frame)
        *(.debug*)
    }
}
