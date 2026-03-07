/**
 * linker.ld  –  Linker-Skript für den PinguinOS-Kernel (x86 32-Bit)
 *
 * Virtuelle Ladeadresse: 1 MB (0x00100000) – Standard für Multiboot-Kernel.
 *
 * Sektionen:
 *   .multiboot  – Multiboot-Header (muss in den ersten 8 KB erscheinen)
 *   .text       – Ausführbarer Code
 *   .rodata     – Schreibgeschützte Daten (Const Strings, Tabellen)
 *   .data       – Initialisierte beschreibbare Daten
 *   .bss        – Uninitialisierte beschreibbare Daten (durch Loader genullt)
 *
 * Der Linker exportiert zwei Symbole für den physischen Speichermanager:
 *   _kernel_start  – Erstes Byte des Kernel-Images (physisch)
 *   _kernel_end    – Erstes Byte nach dem Kernel-Image (physisch),
 *                    aufgerundet auf eine 4 KB Seitengrenze
 */

ENTRY(_start)

SECTIONS
{
    /* ── Ladeadresse ─────────────────────────────────────────────── */
    . = 0x00100000;

    _kernel_start = .;

    /* ── Multiboot-Header (erste 8 KB) ───────────────────────────── */
    .multiboot ALIGN(4) :
    {
        *(.multiboot)
    }

    /* ── Code ────────────────────────────────────────────────────── */
    .text ALIGN(4096) :
    {
        *(.text .text.*)
    }

    /* ── Schreibgeschützte Daten ─────────────────────────────────── */
    .rodata ALIGN(4096) :
    {
        *(.rodata .rodata.*)
    }

    /* ── Initialisierte Daten ────────────────────────────────────── */
    .data ALIGN(4096) :
    {
        *(.data .data.*)
    }

    /* ── Uninitialisierte Daten (BSS) ────────────────────────────── */
    .bss ALIGN(4096) :
    {
        *(COMMON)
        *(.bss .bss.*)
    }

    /* ── End-Markierung (Seiten-ausgerichtet) ─────────────────────── */
    . = ALIGN(4096);
    _kernel_end = .;

    /* ── Debug- und andere Sektionen verwerfen ───────────────────── */
    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
        *(.eh_frame)
        *(.debug*)
    }
}
