/**
 * @file nvme.h
 * @brief NVMe-Treiber für PinguinOS – Feature #43.
 *
 * Unterstützt NVMe-Geräte (PCIe-SSDs) über den NVM Express-Standard.
 * Implementiert:
 *   – Controller-Initialisierung (CAP/CC/CSTS-Register)
 *   – Admin-Queue (Identify, Create I/O Queue)
 *   – I/O-Submission- und Completion-Queue für Namespace 1
 *   – Synchrones Lesen und Schreiben von 512-Byte-Sektoren
 *
 * PCI-Kennzeichnung: Klasse 0x01, Unterklasse 0x08, Prog-IF 0x02
 */
#ifndef _NVME_H
#define _NVME_H

#include "types.h"

/* ── NVMe-Registerofffsets (MMIO, BAR0) ─────────────────────────── */
#define NVME_CAP     0x00   /* Controller Capabilities (64-Bit)      */
#define NVME_VS      0x08   /* Version                               */
#define NVME_INTMS   0x0C   /* Interrupt Mask Set                    */
#define NVME_INTMC   0x10   /* Interrupt Mask Clear                  */
#define NVME_CC      0x14   /* Controller Configuration              */
#define NVME_CSTS    0x1C   /* Controller Status                     */
#define NVME_AQA     0x24   /* Admin Queue Attributes                */
#define NVME_ASQ     0x28   /* Admin Submission Queue Basisadresse   */
#define NVME_ACQ     0x30   /* Admin Completion Queue Basisadresse   */

/* ── CC-Register-Bits ────────────────────────────────────────────── */
#define NVME_CC_EN    (1u << 0)   /* Controller Enable                */
#define NVME_CC_CSS   (0u << 4)   /* Command Set: NVM                 */
#define NVME_CC_MPS   (0u << 7)   /* Memory Page Size: 4 KB           */
#define NVME_CC_AMS   (0u << 11)  /* Arbitration: Round-Robin         */
#define NVME_CC_SHN   (0u << 14)  /* Kein Shutdown                    */
#define NVME_CC_IOSQES (6u << 16) /* I/O SQ Entry Size: 2^6 = 64 B   */
#define NVME_CC_IOCQES (4u << 20) /* I/O CQ Entry Size: 2^4 = 16 B   */

/* ── CSTS-Register-Bits ──────────────────────────────────────────── */
#define NVME_CSTS_RDY  (1u << 0)  /* Controller Ready                 */
#define NVME_CSTS_CFS  (1u << 1)  /* Controller Fatal Status          */

/* ── Queue-Größen ────────────────────────────────────────────────── */
#define NVME_ADMIN_QUEUE_SIZE  16   /* Einträge in Admin-Queue         */
#define NVME_IO_QUEUE_SIZE     16   /* Einträge in I/O-Queues          */

/* ── NVMe-Opcodes ────────────────────────────────────────────────── */
/* Admin-Befehle */
#define NVME_ADMIN_DELETE_SQ    0x00
#define NVME_ADMIN_CREATE_SQ    0x01
#define NVME_ADMIN_GET_LOG_PAGE 0x02
#define NVME_ADMIN_DELETE_CQ    0x04
#define NVME_ADMIN_CREATE_CQ    0x05
#define NVME_ADMIN_IDENTIFY     0x06

/* I/O-Befehle */
#define NVME_IO_FLUSH   0x00
#define NVME_IO_WRITE   0x01
#define NVME_IO_READ    0x02

/* ── Submission-Queue-Eintrag (64 Byte) ──────────────────────────── */
typedef struct PACKED {
    uint32_t cdw0;    /* Command Dword 0 (Opcode, FUSE, CID)         */
    uint32_t nsid;    /* Namespace ID                                 */
    uint64_t rsvd;
    uint64_t mptr;    /* Metadata-Zeiger                             */
    uint64_t prp1;    /* Physical Region Page 1                      */
    uint64_t prp2;    /* Physical Region Page 2                      */
    uint32_t cdw10;   /* Command-spezifisch                          */
    uint32_t cdw11;
    uint32_t cdw12;
    uint32_t cdw13;
    uint32_t cdw14;
    uint32_t cdw15;
} nvme_sqe_t;

/* ── Completion-Queue-Eintrag (16 Byte) ──────────────────────────── */
typedef struct PACKED {
    uint32_t dw0;     /* Befehlsspezifisch                           */
    uint32_t dw1;     /* Reserviert                                  */
    uint16_t sq_head; /* SQ-Kopf-Zeiger                              */
    uint16_t sq_id;   /* SQ-ID                                       */
    uint16_t cmd_id;  /* Befehls-ID                                  */
    uint16_t status;  /* Status-Feld (Bit 0: Phase)                  */
} nvme_cqe_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief NVMe-Treiber initialisieren.
 *
 * Sucht NVMe-Geräte via PCI (Klasse 0x01, Unterklasse 0x08),
 * initialisiert den Controller und richtet die I/O-Queues ein.
 * @return true bei Erfolg, false wenn kein NVMe-Gerät gefunden.
 */
bool nvme_init(void);

/**
 * @brief Sektoren von NVMe-Namespace 1 lesen.
 *
 * @param lba    Logische Blockadresse (Startsektor).
 * @param count  Anzahl zu lesender Sektoren (512 Byte je Sektor).
 * @param buf    Zielpuffer (muss ≥ count × 512 Byte groß sein).
 * @return 0 bei Erfolg, <0 bei Fehler.
 */
int nvme_read(uint64_t lba, uint32_t count, void *buf);

/**
 * @brief Sektoren auf NVMe-Namespace 1 schreiben.
 *
 * @param lba    Logische Blockadresse (Startsektor).
 * @param count  Anzahl zu schreibender Sektoren.
 * @param buf    Quellpuffer.
 * @return 0 bei Erfolg, <0 bei Fehler.
 */
int nvme_write(uint64_t lba, uint32_t count, const void *buf);

/** @return Sektorgröße in Bytes (typisch 512 oder 4096). */
uint32_t nvme_sector_size(void);

/** @return Gesamtzahl der Sektoren in Namespace 1. */
uint64_t nvme_sector_count(void);

/** NVMe-Status und Identify-Daten auf seriell ausgeben. */
void nvme_dump(void);

#endif /* _NVME_H */