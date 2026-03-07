/**
 * @file partition.h
 * @brief MBR and GPT partition table parser API for PinguinOS.
 *
 * Feature #55 – MBR/GPT Parsing
 */
#ifndef _PARTITION_H
#define _PARTITION_H

#include "types.h"

#define PART_MAX         128    /* Max partitions (GPT allows 128)  */
#define PART_TYPE_EMPTY  0x00
#define PART_TYPE_FAT32  0x0B   /* FAT32 with CHS addressing       */
#define PART_TYPE_FAT32L 0x0C   /* FAT32 with LBA                  */
#define PART_TYPE_EXT4   0x83   /* Linux native                    */
#define PART_TYPE_SWAP   0x82
#define PART_TYPE_GPT    0xEE   /* GPT protective MBR entry        */

typedef struct {
    uint8_t  type;        /* MBR partition type byte        */
    uint64_t lba_start;   /* First LBA sector               */
    uint64_t lba_size;    /* Number of sectors              */
    bool     bootable;
    char     label[36];   /* GPT partition name (UTF-8)     */
} partition_t;

typedef struct {
    uint32_t    count;
    bool        is_gpt;
    partition_t parts[PART_MAX];
} partition_table_t;

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Parse the partition table from IDE drive @p drive.
 *
 * Auto-detects whether the disk uses MBR or GPT.
 * @return 0 on success, -1 if no valid table found.
 */
int partition_read(uint8_t drive, partition_table_t *table);

/** Print the partition table to the serial console. */
void partition_dump(const partition_table_t *table);

#endif /* _PARTITION_H */