/**
 * @file ide.h
 * @brief IDE/PATA-Treiber (PIO-Mode) für PinguinOS (Feature #41).
 *
 * Unterstützt primären und sekundären IDE-Controller (IRQ14/IRQ15).
 * Lesen/Schreiben erfolgt im 28-Bit-LBA-Modus über PIO.
 */
#ifndef _IDE_H
#define _IDE_H

#include "types.h"

/* ── IDE I/O-Ports ───────────────────────────────────────────────── */
#define IDE_PRI_BASE   0x1F0   /* Primärer Controller Data-Register  */
#define IDE_PRI_CTRL   0x3F6   /* Primärer Device-Control-Register   */
#define IDE_SEC_BASE   0x170   /* Sekundärer Controller              */
#define IDE_SEC_CTRL   0x376

/* Register-Offsets ab Base */
#define IDE_DATA       0x00
#define IDE_ERROR      0x01
#define IDE_FEATURES   0x01
#define IDE_NSEC       0x02
#define IDE_LBA_LO     0x03
#define IDE_LBA_MI     0x04
#define IDE_LBA_HI     0x05
#define IDE_SELECT     0x06
#define IDE_STATUS     0x07
#define IDE_CMD        0x07

/* Status-Bits */
#define IDE_BSY   0x80
#define IDE_DRDY  0x40
#define IDE_DRQ   0x08
#define IDE_ERR   0x01

/* Befehle */
#define IDE_CMD_READ    0x20
#define IDE_CMD_WRITE   0x30
#define IDE_CMD_IDENT   0xEC   /* IDENTIFY DEVICE */
#define IDE_CMD_FLUSH   0xE7

#define IDE_SECTOR_SIZE 512
#define IDE_MAX_DRIVES  4

/* ── Laufwerks-Deskriptor ────────────────────────────────────────── */
typedef struct {
    bool     exists;
    uint16_t base;         /* I/O-Basisport             */
    uint16_t ctrl;         /* Control-Register-Port     */
    uint8_t  drive;        /* 0 = Master, 1 = Slave     */
    uint8_t  channel;      /* 0 = primär, 1 = sekundär  */
    uint32_t lba28_sectors;/* Kapazität in Sektoren     */
    char     model[41];    /* Modell-String aus IDENTIFY */
} ide_drive_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */
void ide_init(void);

/**
 * @brief Liest @p count Sektoren ab LBA @p lba in @p buf.
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
int ide_read(uint8_t drive, uint32_t lba, uint8_t count, uint8_t *buf);

/**
 * @brief Schreibt @p count Sektoren ab LBA @p lba aus @p buf.
 */
int ide_write(uint8_t drive, uint32_t lba, uint8_t count, const uint8_t *buf);

const ide_drive_t *ide_get_drives(void);
uint32_t           ide_drive_count(void);
void               ide_dump(void);

#endif /* _IDE_H */
