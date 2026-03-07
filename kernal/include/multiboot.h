/**
 * @file multiboot.h
 * @brief Multiboot 1 specification structures.
 *
 * When GRUB loads PinguinOS it places a pointer to a multiboot_info_t
 * structure in EBX and a magic value (MULTIBOOT_MAGIC) in EAX before
 * jumping to the kernel entry point.
 */
#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "types.h"

/* ── Magic values ─────────────────────────────────────────────────── */
#define MULTIBOOT_MAGIC         0x2BADB002   /* Value in EAX on entry  */
#define MULTIBOOT_HEADER_MAGIC  0x1BADB002   /* Value in the header    */

/* ── Header flags ────────────────────────────────────────────────── */
#define MULTIBOOT_FLAG_ALIGN    (1 << 0)     /* Align modules on pages */
#define MULTIBOOT_FLAG_MEMMAP   (1 << 1)     /* Request memory map     */
#define MULTIBOOT_FLAG_VIDEO    (1 << 2)     /* Request video info     */

/* ── Info flags (set by bootloader) ─────────────────────────────── */
#define MULTIBOOT_INFO_MEM      (1 << 0)
#define MULTIBOOT_INFO_MMAP     (1 << 6)
#define MULTIBOOT_INFO_FB       (1 << 12)

/* ── Memory map entry types ─────────────────────────────────────── */
#define MULTIBOOT_MMAP_AVAILABLE   1
#define MULTIBOOT_MMAP_RESERVED    2

/* ── Multiboot header (must appear in the first 8 KB of the kernel) */
typedef struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    /* optional fields (only present when flags[16] set) */
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((packed)) multiboot_header_t;

/* ── Memory map entry ────────────────────────────────────────────── */
typedef struct multiboot_mmap_entry {
    uint32_t size;          /* Size of this entry (not including this field) */
    uint64_t addr;          /* Base address of the region                    */
    uint64_t len;           /* Length of the region in bytes                 */
    uint32_t type;          /* 1 = available, else reserved/bad              */
} __attribute__((packed)) multiboot_mmap_entry_t;

/* ── Framebuffer info ────────────────────────────────────────────── */
typedef struct multiboot_framebuffer_info {
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;   /* 1=indexed, 2=RGB, 0=EGA text */
} __attribute__((packed)) multiboot_fb_t;

/* ── Full Multiboot information structure ────────────────────────── */
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;          /* KB below 1 MB                   */
    uint32_t mem_upper;          /* KB above 1 MB                   */
    uint32_t boot_device;
    uint32_t cmdline;            /* Physical addr of command line   */
    uint32_t mods_count;
    uint32_t mods_addr;
    uint8_t  syms[16];           /* Symbol table info (varies)      */
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    /* VBE info */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    /* Framebuffer info */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} __attribute__((packed)) multiboot_info_t;

#endif /* _MULTIBOOT_H */
