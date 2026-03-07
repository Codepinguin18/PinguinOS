/**
 * @file partition.c
 * @brief MBR- und GPT-Partitionstabellen-Parser für PinguinOS.
 *
 * Feature #55 – MBR/GPT Parsing
 *
 * MBR: Sektor 0, Magic 0x55AA, bis zu 4 primäre Partitionen ab Offset 446.
 * GPT: MBR-Sektor 0 Typ=0xEE, GPT-Header bei LBA 1, Einträge ab LBA 2.
 */

#include "../include/partition.h"
#include "../include/ide.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── MBR-Layout auf der Disk ─────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    uint8_t  status;         /* 0x80 = bootfähig    */
    uint8_t  chs_first[3];
    uint8_t  type;
    uint8_t  chs_last[3];
    uint32_t lba_start;
    uint32_t lba_size;
} mbr_entry_t;

typedef struct __attribute__((packed)) {
    uint8_t    bootstrap[446];
    mbr_entry_t parts[4];
    uint16_t   signature;    /* 0x55AA */
} mbr_t;

/* ── GPT-Layout auf der Disk ─────────────────────────────────────── */
#define GPT_SIGNATURE  0x5452415020494645ULL   /* "EFI PART" */

typedef struct __attribute__((packed)) {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    uint8_t  disk_guid[16];
    uint64_t partition_entry_lba;
    uint32_t num_partition_entries;
    uint32_t partition_entry_size;
    uint32_t partition_array_crc32;
} gpt_header_t;

typedef struct __attribute__((packed)) {
    uint8_t  type_guid[16];
    uint8_t  unique_guid[16];
    uint64_t start_lba;
    uint64_t end_lba;
    uint64_t attributes;
    uint16_t name[36];   /* UTF-16LE */
} gpt_entry_t;

/* ── Hilfsfunktion: UTF-16LE nach ASCII (Best-Effort) ────────────── */
static void utf16_to_ascii(const uint16_t *src, char *dst, uint32_t max)
{
    uint32_t i = 0;
    while (i + 1 < max && src[i]) {
        dst[i] = (src[i] < 0x80) ? (char)src[i] : '?';
        i++;
    }
    dst[i] = '\0';
}

/* ── Prüfen, ob eine GUID nur aus Nullen besteht ────────────────── */
static bool guid_is_zero(const uint8_t *g)
{
    for (int i = 0; i < 16; i++) if (g[i]) return false;
    return true;
}

/* ── Öffentlich: partition_read ──────────────────────────────────── */
int partition_read(uint8_t drive, partition_table_t *table)
{
    if (!table) return -1;
    memset(table, 0, sizeof(*table));

    uint8_t sector[IDE_SECTOR_SIZE];

    /* MBR lesen (Sektor 0) */
    if (ide_read(drive, 0, 1, sector) != 0) {
        KERR("partition: MBR von Laufwerk %u konnte nicht gelesen werden\n", drive);
        return -1;
    }

    mbr_t *mbr = (mbr_t *)sector;
    if (mbr->signature != 0xAA55) {
        KERR("partition: keine valide MBR-Signatur auf Laufwerk %u\n", drive);
        return -1;
    }

    /* Auf GPT-Schutz-MBR prüfen */
    bool gpt_present = false;
    for (int i = 0; i < 4; i++) {
        if (mbr->parts[i].type == PART_TYPE_GPT) {
            gpt_present = true;
            break;
        }
    }

    if (gpt_present) {
        /* ── GPT ────────────────────────────────────────────────── */
        table->is_gpt = true;

        uint8_t hdr_buf[IDE_SECTOR_SIZE];
        if (ide_read(drive, 1, 1, hdr_buf) != 0) return -1;

        gpt_header_t *hdr = (gpt_header_t *)hdr_buf;
        if (hdr->signature != GPT_SIGNATURE) return -1;

        uint32_t max_entries = hdr->num_partition_entries;
        if (max_entries > PART_MAX) max_entries = PART_MAX;
        uint32_t entry_size  = hdr->partition_entry_size;
        uint64_t entry_lba   = hdr->partition_entry_lba;

        /* Bis zu PART_MAX Einträge lesen */
        uint32_t sectors_needed = (max_entries * entry_size + IDE_SECTOR_SIZE - 1)
                                   / IDE_SECTOR_SIZE;
        if (sectors_needed > 32) sectors_needed = 32;

        /* Temporären Puffer auf dem Heap allokieren */
        uint32_t buf_size = sectors_needed * IDE_SECTOR_SIZE;
        uint8_t *entry_buf = (uint8_t *)kmalloc(buf_size);
        if (!entry_buf) return -1;

        if (ide_read(drive, (uint32_t)entry_lba, (uint8_t)sectors_needed,
                     entry_buf) != 0) {
            kfree(entry_buf);
            return -1;
        }

        for (uint32_t i = 0; i < max_entries; i++) {
            gpt_entry_t *e = (gpt_entry_t *)(entry_buf + i * entry_size);
            if (guid_is_zero(e->type_guid)) continue;

            partition_t *p = &table->parts[table->count++];
            p->lba_start = e->start_lba;
            p->lba_size  = e->end_lba - e->start_lba + 1;
            p->type      = 0;   /* GPT nutzt keinen MBR-Typ */
            p->bootable  = !!(e->attributes & 4);   /* Erforderliches Partitions-Bit */
            
            /* Fix: -Waddress-of-packed-member durch lokales Kopieren */
            uint16_t name_tmp[36];
            memcpy(name_tmp, e->name, sizeof(name_tmp));
            utf16_to_ascii(name_tmp, p->label, sizeof(p->label));
        }
        kfree(entry_buf);

    } else {
        /* ── MBR ────────────────────────────────────────────────── */
        table->is_gpt = false;
        for (int i = 0; i < 4; i++) {
            mbr_entry_t *e = &mbr->parts[i];
            if (e->type == PART_TYPE_EMPTY) continue;

            partition_t *p = &table->parts[table->count++];
            p->type      = e->type;
            p->lba_start = e->lba_start;
            p->lba_size  = e->lba_size;
            p->bootable  = (e->status == 0x80);
            snprintf(p->label, sizeof(p->label), "part%d", i + 1);
        }
    }

    KINFO("partition: Laufwerk %u  %s  %u Partition(en)\n",
          drive, table->is_gpt ? "GPT" : "MBR", table->count);
    return 0;
}

/* ── Öffentlich: partition_dump ──────────────────────────────────── */
void partition_dump(const partition_table_t *table)
{
    if (!table) return;
    char buf[128];
    snprintf(buf, sizeof(buf), "[PART] %s Tabelle  %u Eintraege\n",
             table->is_gpt ? "GPT" : "MBR", table->count);
    serial_puts(buf);

    for (uint32_t i = 0; i < table->count; i++) {
        const partition_t *p = &table->parts[i];
        snprintf(buf, sizeof(buf),
                 "  [%u] type=0x%02x  lba=%u  sectors=%u  boot=%d  '%s'\n",
                 i, p->type,
                 (uint32_t)p->lba_start,
                 (uint32_t)p->lba_size,
                 p->bootable ? 1 : 0,
                 p->label);
        serial_puts(buf);
    }
}
