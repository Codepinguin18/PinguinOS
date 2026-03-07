/**
 * @file multiboot.h
 * @brief Multiboot 1 Spezifikations-Strukturen.
 *
 * Wenn GRUB PinguinOS lädt, platziert es beim Sprung zum Kernel-Einstiegspunkt
 * einen Zeiger auf eine multiboot_info_t-Struktur in EBX und einen magischen
 * Wert (MULTIBOOT_MAGIC) in EAX.
 */
#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "types.h"

/* ── Magische Werte ─────────────────────────────────────────────── */
#define MULTIBOOT_MAGIC         0x2BADB002   /* Wert in EAX beim Einstieg */
#define MULTIBOOT_HEADER_MAGIC  0x1BADB002   /* Wert im Header            */

/* ── Header-Flags ────────────────────────────────────────────────── */
#define MULTIBOOT_FLAG_ALIGN    (1 << 0)     /* Module an Seiten ausrichten */
#define MULTIBOOT_FLAG_MEMMAP   (1 << 1)     /* Memory-Map anfordern        */
#define MULTIBOOT_FLAG_VIDEO    (1 << 2)     /* Video-Info anfordern        */

/* ── Info-Flags (gesetzt durch den Bootloader) ───────────────────── */
#define MULTIBOOT_INFO_MEM      (1 << 0)
#define MULTIBOOT_INFO_MMAP     (1 << 6)
#define MULTIBOOT_INFO_FB       (1 << 12)

/* ── Memory-Map Eintragstypen ────────────────────────────────────── */
#define MULTIBOOT_MMAP_AVAILABLE   1
#define MULTIBOOT_MMAP_RESERVED    2

/* ── Multiboot-Header (muss in den ersten 8 KB des Kernels erscheinen) */
typedef struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    /* optionale Felder (nur vorhanden, wenn flags[16] gesetzt ist) */
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((packed)) multiboot_header_t;

/* ── Memory-Map Eintrag ──────────────────────────────────────────── */
typedef struct multiboot_mmap_entry {
    uint32_t size;          /* Größe dieses Eintrags (ohne dieses Feld)      */
    uint64_t addr;          /* Basisadresse der Region                       */
    uint64_t len;           /* Länge der Region in Bytes                     */
    uint32_t type;          /* 1 = verfügbar, sonst reserviert/defekt        */
} __attribute__((packed)) multiboot_mmap_entry_t;

/* ── Framebuffer-Info ────────────────────────────────────────────── */
typedef struct multiboot_framebuffer_info {
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;   /* 1=indiziert, 2=RGB, 0=EGA Text */
} __attribute__((packed)) multiboot_fb_t;

/* ── Vollständige Multiboot-Informationsstruktur ─────────────────── */
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;          /* KB unter 1 MB                    */
    uint32_t mem_upper;          /* KB über 1 MB                     */
    uint32_t boot_device;
    uint32_t cmdline;            /* Physische Adr. der Kommandozeile */
    uint32_t mods_count;
    uint32_t mods_addr;
    uint8_t  syms[16];           /* Symboltabellen-Info (variiert)   */
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    /* VBE Info */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    /* Framebuffer Info */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} __attribute__((packed)) multiboot_info_t;

#endif /* _MULTIBOOT_H */
