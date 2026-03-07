/**
 * @file ide.c
 * @brief IDE/PATA PIO-Mode-Treiber für PinguinOS.
 */
#include "../include/ide.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/idt.h"

static ide_drive_t drives[IDE_MAX_DRIVES];
static uint32_t    drive_count = 0;

/* ── Warten bis nicht busy ───────────────────────────────────────── */
static int ide_wait(uint16_t base, bool drq)
{
    uint8_t status;
    uint32_t timeout = 0x100000;

    while (--timeout) {
        status = inb(base + IDE_STATUS);
        if (status & IDE_ERR) return -1;
        if (status & IDE_BSY) continue;
        if (!drq || (status & IDE_DRQ)) return 0;
    }
    return -1;   /* Timeout */
}

/* ── IDENTIFY DEVICE ────────────────────────────────────────────── */
static bool ide_identify(ide_drive_t *d)
{
    /* Laufwerk auswählen */
    outb(d->base + IDE_SELECT, (uint8_t)(0xA0 | (d->drive << 4)));
    io_wait();

    /* LBA/Sektorzähler auf 0 setzen */
    outb(d->base + IDE_NSEC,   0);
    outb(d->base + IDE_LBA_LO, 0);
    outb(d->base + IDE_LBA_MI, 0);
    outb(d->base + IDE_LBA_HI, 0);

    outb(d->base + IDE_CMD, IDE_CMD_IDENT);
    io_wait();

    uint8_t status = inb(d->base + IDE_STATUS);
    if (!status) return false;   /* Kein Laufwerk */

    if (ide_wait(d->base, true) != 0) return false;

    /* 256 Words (512 Bytes) lesen */
    uint16_t ident[256];
    for (int i = 0; i < 256; i++)
        ident[i] = inw(d->base + IDE_DATA);

    /* LBA28-Kapazität: Words 60–61 */
    d->lba28_sectors = ((uint32_t)ident[61] << 16) | ident[60];

    /* Modell-String: Words 27–46, Big-Endian-Bytes tauschen */
    for (int i = 0; i < 20; i++) {
        d->model[i * 2]     = (char)(ident[27 + i] >> 8);
        d->model[i * 2 + 1] = (char)(ident[27 + i] & 0xFF);
    }
    d->model[40] = '\0';

    /* Führende/Nachfolgende Leerzeichen entfernen */
    for (int i = 39; i >= 0 && d->model[i] == ' '; i--) d->model[i] = '\0';

    d->exists = true;
    return true;
}

/* ── LBA28 auswählen ─────────────────────────────────────────────── */
static void ide_select_lba(ide_drive_t *d, uint32_t lba, uint8_t count)
{
    outb(d->base + IDE_SELECT,
         (uint8_t)(0xE0 | (d->drive << 4) | ((lba >> 24) & 0x0F)));
    outb(d->base + IDE_NSEC,   count);
    outb(d->base + IDE_LBA_LO, (uint8_t)(lba & 0xFF));
    outb(d->base + IDE_LBA_MI, (uint8_t)((lba >> 8) & 0xFF));
    outb(d->base + IDE_LBA_HI, (uint8_t)((lba >> 16) & 0xFF));
}

/* ── Öffentlich: ide_init ────────────────────────────────────────── */
void ide_init(void)
{
    memset(drives, 0, sizeof(drives));
    drive_count = 0;

    /* Software-Reset */
    outb(IDE_PRI_CTRL, 0x04); io_wait();
    outb(IDE_PRI_CTRL, 0x00); io_wait();
    outb(IDE_SEC_CTRL, 0x04); io_wait();
    outb(IDE_SEC_CTRL, 0x00); io_wait();

    /* Alle vier möglichen Laufwerke prüfen */
    typedef struct { uint16_t base; uint16_t ctrl; uint8_t drv; uint8_t ch; }
            drive_cfg_t;
    drive_cfg_t cfg[] = {
        { IDE_PRI_BASE, IDE_PRI_CTRL, 0, 0 },
        { IDE_PRI_BASE, IDE_PRI_CTRL, 1, 0 },
        { IDE_SEC_BASE, IDE_SEC_CTRL, 0, 1 },
        { IDE_SEC_BASE, IDE_SEC_CTRL, 1, 1 },
    };

    for (int i = 0; i < 4; i++) {
        drives[i].base    = cfg[i].base;
        drives[i].ctrl    = cfg[i].ctrl;
        drives[i].drive   = cfg[i].drv;
        drives[i].channel = cfg[i].ch;

        if (ide_identify(&drives[i])) {
            drive_count++;
            KINFO("IDE: Laufwerk %d  %u MB  '%s'\n",
                  i,
                  (drives[i].lba28_sectors / 2048),
                  drives[i].model);
        }
    }

    KINFO("IDE: %u Laufwerk(e) gefunden\n", drive_count);
}

/* ── Lesen ───────────────────────────────────────────────────────── */
int ide_read(uint8_t drv, uint32_t lba, uint8_t count, uint8_t *buf)
{
    if (drv >= IDE_MAX_DRIVES || !drives[drv].exists) return -1;
    ide_drive_t *d = &drives[drv];

    ide_wait(d->base, false);
    ide_select_lba(d, lba, count);
    outb(d->base + IDE_CMD, IDE_CMD_READ);

    for (uint8_t s = 0; s < count; s++) {
        if (ide_wait(d->base, true) != 0) return -1;
        uint16_t *ptr = (uint16_t *)(buf + s * IDE_SECTOR_SIZE);
        for (int i = 0; i < 256; i++) ptr[i] = inw(d->base + IDE_DATA);
    }
    return 0;
}

/* ── Schreiben ───────────────────────────────────────────────────── */
int ide_write(uint8_t drv, uint32_t lba, uint8_t count, const uint8_t *buf)
{
    if (drv >= IDE_MAX_DRIVES || !drives[drv].exists) return -1;
    ide_drive_t *d = &drives[drv];

    ide_wait(d->base, false);
    ide_select_lba(d, lba, count);
    outb(d->base + IDE_CMD, IDE_CMD_WRITE);

    for (uint8_t s = 0; s < count; s++) {
        if (ide_wait(d->base, true) != 0) return -1;
        const uint16_t *ptr = (const uint16_t *)(buf + s * IDE_SECTOR_SIZE);
        for (int i = 0; i < 256; i++) outw(d->base + IDE_DATA, ptr[i]);
    }
    outb(d->base + IDE_CMD, IDE_CMD_FLUSH);
    ide_wait(d->base, false);
    return 0;
}

const ide_drive_t *ide_get_drives(void)  { return drives;      }
uint32_t           ide_drive_count(void) { return drive_count; }

void ide_dump(void)
{
    for (uint32_t i = 0; i < IDE_MAX_DRIVES; i++) {
        if (!drives[i].exists) continue;
        KINFO("IDE %u: %s  Ch%u  %u Sektoren  '%s'\n",
              i,
              drives[i].drive == 0 ? "Master" : "Slave",
              drives[i].channel,
              drives[i].lba28_sectors,
              drives[i].model);
    }
}
