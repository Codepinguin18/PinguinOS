/**
 * @file multiboot.h
 * @brief Multiboot 1-Spezifikations-Strukturen für PinguinOS.
 *
 * Wenn GRUB PinguinOS lädt, legt es einen Zeiger auf multiboot_info_t
 * in EBX und den Magic-Wert (MULTIBOOT_MAGIC) in EAX ab, bevor es
 * zum Kernel-Einstiegspunkt springt.
 */
#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "types.h"

/* ── Magic-Werte ─────────────────────────────────────────────────── */
#define MULTIBOOT_MAGIC         0x2BADB002   /* Wert in EAX beim Start */
#define MULTIBOOT_HEADER_MAGIC  0x1BADB002   /* Wert im Header         */

/* ── Header-Flags ────────────────────────────────────────────────── */
#define MULTIBOOT_FLAG_ALIGN    (1 << 0)     /* Module seitenausrichten */
#define MULTIBOOT_FLAG_MEMMAP   (1 << 1)     /* Speicherkarte anfragen  */
#define MULTIBOOT_FLAG_VIDEO    (1 << 2)     /* Videoinfo anfragen      */

/* ── Info-Flags (vom Bootloader gesetzt) ─────────────────────────── */
#define MULTIBOOT_INFO_MEM      (1 << 0)
#define MULTIBOOT_INFO_MMAP     (1 << 6)
#define MULTIBOOT_INFO_FB       (1 << 12)

/* ── Speicherkarten-Eintragstypen ────────────────────────────────── */
#define MULTIBOOT_MMAP_AVAILABLE   1
#define MULTIBOOT_MMAP_RESERVED    2

/* ── Multiboot-Header (muss in den ersten 8 KB des Kernels liegen) ─ */
typedef struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((packed)) multiboot_header_t;

/* ── Speicherkarten-Eintrag ──────────────────────────────────────── */
typedef struct multiboot_mmap_entry {
    uint32_t size;   /* Größe dieses Eintrags (ohne dieses Feld)      */
    uint64_t addr;   /* Physische Basisadresse des Bereichs           */
    uint64_t len;    /* Länge des Bereichs in Bytes                   */
    uint32_t type;   /* 1 = verfügbar, sonst reserviert/fehlerhaft    */
} __attribute__((packed)) multiboot_mmap_entry_t;

/* ── Modul-Eintrag ───────────────────────────────────────────────── */
/*
 * GRUB hinterlegt für jedes geladene Modul einen solchen Eintrag.
 * mods_addr in multiboot_info_t zeigt auf ein Array dieser Struktur;
 * mods_count gibt die Anzahl der Einträge an.
 */
typedef struct multiboot_module {
    uint32_t mod_start;   /* Physische Startadresse des Modulpuffers  */
    uint32_t mod_end;     /* Physische Endadresse (exklusiv)          */
    uint32_t cmdline;     /* Physische Adresse der Kommandozeile      */
    uint32_t reserved;    /* Muss 0 sein                              */
} __attribute__((packed)) multiboot_module_t;

/* ── Framebuffer-Info ────────────────────────────────────────────── */
typedef struct multiboot_framebuffer_info {
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;   /* 1=indiziert, 2=RGB, 0=EGA-Text   */
} __attribute__((packed)) multiboot_fb_t;

/* ── Vollständige Multiboot-Informationsstruktur ─────────────────── */
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;           /* KB unterhalb 1 MB                */
    uint32_t mem_upper;           /* KB oberhalb  1 MB                */
    uint32_t boot_device;
    uint32_t cmdline;             /* Physische Adresse der Kernel-CL  */
    uint32_t mods_count;          /* Anzahl geladener Module          */
    uint32_t mods_addr;           /* Zeiger auf multiboot_module_t[]  */
    uint8_t  syms[16];            /* Symboltabellen-Info              */
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    /* VBE-Informationen */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    /* Framebuffer-Informationen */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} __attribute__((packed)) multiboot_info_t;

#endif /* _MULTIBOOT_H */