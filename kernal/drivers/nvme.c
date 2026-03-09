/**
 * @file nvme.c
 * @brief NVMe-Treiber für PinguinOS – Feature #43.
 *
 * Implementiert den NVM-Express-Controller-Treiber für PCIe-SSDs:
 *   – MMIO-Mapping des NVMe-Registers (BAR0)
 *   – Admin-Queue-Setup und Controller-Initialisierung
 *   – Identify-Befehl (Controller + Namespace)
 *   – I/O-Queue-Erstellung (CREATE_SQ / CREATE_CQ)
 *   – Synchrones Lesen und Schreiben über I/O-Queues
 */

#include "../include/nvme.h"
#include "../include/pci.h"
#include "../include/paging.h"
#include "../include/mm.h"    /* PAGE_SIZE */
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── PCI-Kennung für NVMe ────────────────────────────────────────── */
#define NVME_PCI_CLASS    0x01
#define NVME_PCI_SUBCLASS 0x08

/* ── Modul-Zustand ───────────────────────────────────────────────── */
static volatile uint32_t *nvme_regs = NULL;  /* MMIO-Basisadresse    */
static bool               nvme_ok   = false;
static uint32_t           nvme_db_stride;    /* Doorbell-Stride       */
static uint32_t           nvme_page_size;    /* Controller-Seitengröße */
static uint32_t           nvme_sector_sz = 512;
static uint64_t           nvme_sectors   = 0;

/* ── Admin- und I/O-Queues ───────────────────────────────────────── */
static nvme_sqe_t *admin_sq  = NULL;   /* Admin-Submission-Queue     */
static nvme_cqe_t *admin_cq  = NULL;   /* Admin-Completion-Queue     */
static nvme_sqe_t *io_sq     = NULL;   /* I/O-Submission-Queue       */
static nvme_cqe_t *io_cq     = NULL;   /* I/O-Completion-Queue       */

static uint32_t admin_sq_tail = 0;
static uint32_t admin_cq_head = 0;
static uint8_t  admin_phase   = 1;
static uint32_t io_sq_tail    = 0;
static uint32_t io_cq_head    = 0;
static uint8_t  io_phase      = 1;
static uint16_t cmd_id        = 0;

/* ── Register-Zugriff ────────────────────────────────────────────── */
static INLINE uint32_t nvme_readl(uint32_t off)
{
    return nvme_regs[off / 4];
}
static INLINE void nvme_writel(uint32_t off, uint32_t val)
{
    nvme_regs[off / 4] = val;
}
static INLINE uint64_t nvme_readq(uint32_t off)
{
    return (uint64_t)nvme_regs[off/4] | ((uint64_t)nvme_regs[off/4+1] << 32);
}
static INLINE void nvme_writeq(uint32_t off, uint64_t val)
{
    nvme_regs[off/4]   = (uint32_t)(val);
    nvme_regs[off/4+1] = (uint32_t)(val >> 32);
}

/* ── Doorbell-Schreibzugriff ─────────────────────────────────────── */
/* Doorbell-Offset: 0x1000 + 2 * qid * 4 * stride (SQ),
 *                  0x1000 + (2 * qid + 1) * 4 * stride (CQ) */
static INLINE void nvme_ring_sq_db(uint16_t qid, uint32_t tail)
{
    uint32_t off = 0x1000 + (2u * qid) * 4u * nvme_db_stride;
    nvme_writel(off, tail);
}
static INLINE void nvme_ring_cq_db(uint16_t qid, uint32_t head)
{
    uint32_t off = 0x1000 + (2u * qid + 1u) * 4u * nvme_db_stride;
    nvme_writel(off, head);
}

/* ── Admin-Befehl einreichen und auf Completion warten ───────────── */
static int nvme_admin_submit(nvme_sqe_t *sqe)
{
    uint16_t cid = cmd_id++;
    sqe->cdw0 = (sqe->cdw0 & 0xFFFF) | ((uint32_t)cid << 16);

    /* SQE in Admin-Queue schreiben */
    admin_sq[admin_sq_tail] = *sqe;
    admin_sq_tail = (admin_sq_tail + 1) % NVME_ADMIN_QUEUE_SIZE;
    nvme_ring_sq_db(0, admin_sq_tail);

    /* Auf Phase-Bit in Completion-Queue warten */
    uint32_t timeout = 1000000;
    while (timeout--) {
        nvme_cqe_t *cqe = &admin_cq[admin_cq_head];
        if ((cqe->status & 1) == admin_phase) {
            uint16_t sc = (cqe->status >> 1) & 0xFF;
            admin_cq_head = (admin_cq_head + 1) % NVME_ADMIN_QUEUE_SIZE;
            if (admin_cq_head == 0) admin_phase ^= 1;
            nvme_ring_cq_db(0, admin_cq_head);
            return (sc == 0) ? 0 : -1;
        }
        __asm__ volatile ("pause");
    }
    KERR("NVMe: Admin-Befehl Timeout\n");
    return -1;
}

/* ── I/O-Befehl einreichen ───────────────────────────────────────── */
static int nvme_io_submit(nvme_sqe_t *sqe)
{
    uint16_t cid = cmd_id++;
    sqe->cdw0 = (sqe->cdw0 & 0xFFFF) | ((uint32_t)cid << 16);

    io_sq[io_sq_tail] = *sqe;
    io_sq_tail = (io_sq_tail + 1) % NVME_IO_QUEUE_SIZE;
    nvme_ring_sq_db(1, io_sq_tail);

    uint32_t timeout = 2000000;
    while (timeout--) {
        nvme_cqe_t *cqe = &io_cq[io_cq_head];
        if ((cqe->status & 1) == io_phase) {
            uint16_t sc = (cqe->status >> 1) & 0xFF;
            io_cq_head = (io_cq_head + 1) % NVME_IO_QUEUE_SIZE;
            if (io_cq_head == 0) io_phase ^= 1;
            nvme_ring_cq_db(1, io_cq_head);
            return (sc == 0) ? 0 : -1;
        }
        __asm__ volatile ("pause");
    }
    KERR("NVMe: I/O-Befehl Timeout\n");
    return -1;
}

/* ── Öffentlich: nvme_init ───────────────────────────────────────── */
bool nvme_init(void)
{
    /* PCI-Gerät suchen */
    pci_device_t *pci_dev = pci_find_class(NVME_PCI_CLASS, NVME_PCI_SUBCLASS);
    if (!pci_dev) {
        KINFO("NVMe: Kein Gerät gefunden\n");
        return false;
    }

    /* MMIO-BAR0 mappen */
    uint32_t mmio_phys = pci_dev->bar[0] & ~0xFu;
    if (!mmio_phys) { KERR("NVMe: BAR0 ungültig\n"); return false; }

    /* Seitenweise identitätsmappen (128 KB) */
    for (uint32_t off = 0; off < 0x20000; off += PAGE_SIZE)
        paging_map_page(mmio_phys + off, mmio_phys + off,
                        PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);
    nvme_regs = (volatile uint32_t *)mmio_phys;

    pci_enable_bus_master(pci_dev);

    /* CAP auslesen: Doorbell-Stride und unterstützte Seitengröße */
    uint64_t cap      = nvme_readq(NVME_CAP);
    nvme_db_stride    = 1u << (((cap >> 32) & 0xF) + 2);
    uint32_t mpsmin   = (uint32_t)(1u << (12 + ((cap >> 48) & 0xF)));
    nvme_page_size    = mpsmin;

    /* Controller deaktivieren (CC.EN = 0) */
    nvme_writel(NVME_CC, nvme_readl(NVME_CC) & ~NVME_CC_EN);
    uint32_t timeout = 100000;
    while ((nvme_readl(NVME_CSTS) & NVME_CSTS_RDY) && timeout--);

    /* Admin-Queues allozieren (4-KB-ausgerichtet) */
    admin_sq = (nvme_sqe_t *)kmalloc(NVME_ADMIN_QUEUE_SIZE * sizeof(nvme_sqe_t));
    admin_cq = (nvme_cqe_t *)kmalloc(NVME_ADMIN_QUEUE_SIZE * sizeof(nvme_cqe_t));
    if (!admin_sq || !admin_cq) { KERR("NVMe: Speicher fehlt\n"); return false; }
    memset(admin_sq, 0, NVME_ADMIN_QUEUE_SIZE * sizeof(nvme_sqe_t));
    memset(admin_cq, 0, NVME_ADMIN_QUEUE_SIZE * sizeof(nvme_cqe_t));

    /* Queue-Attribute und Basisadressen setzen */
    nvme_writel(NVME_AQA,
                ((NVME_ADMIN_QUEUE_SIZE - 1) << 16) |
                (NVME_ADMIN_QUEUE_SIZE - 1));
    nvme_writeq(NVME_ASQ, (uint64_t)(uint32_t)admin_sq);
    nvme_writeq(NVME_ACQ, (uint64_t)(uint32_t)admin_cq);

    /* Controller starten */
    nvme_writel(NVME_CC,
                NVME_CC_EN | NVME_CC_CSS | NVME_CC_MPS |
                NVME_CC_AMS | NVME_CC_SHN |
                NVME_CC_IOSQES | NVME_CC_IOCQES);

    /* Auf Ready warten */
    timeout = 2000000;
    while (!(nvme_readl(NVME_CSTS) & NVME_CSTS_RDY) && timeout--);
    if (!timeout) { KERR("NVMe: Controller-Start Timeout\n"); return false; }
    if (nvme_readl(NVME_CSTS) & NVME_CSTS_CFS) {
        KERR("NVMe: Controller Fatal Status\n");
        return false;
    }

    /* I/O-Queues allozieren */
    io_sq = (nvme_sqe_t *)kmalloc(NVME_IO_QUEUE_SIZE * sizeof(nvme_sqe_t));
    io_cq = (nvme_cqe_t *)kmalloc(NVME_IO_QUEUE_SIZE * sizeof(nvme_cqe_t));
    if (!io_sq || !io_cq) { KERR("NVMe: I/O-Queue-Speicher fehlt\n"); return false; }
    memset(io_sq, 0, NVME_IO_QUEUE_SIZE * sizeof(nvme_sqe_t));
    memset(io_cq, 0, NVME_IO_QUEUE_SIZE * sizeof(nvme_cqe_t));

    /* I/O-Completion-Queue erstellen (Admin-Befehl 0x05) */
    nvme_sqe_t sqe;
    memset(&sqe, 0, sizeof(sqe));
    sqe.cdw0  = NVME_ADMIN_CREATE_CQ;
    sqe.prp1  = (uint64_t)(uint32_t)io_cq;
    sqe.cdw10 = ((NVME_IO_QUEUE_SIZE - 1) << 16) | 1; /* QID=1 */
    sqe.cdw11 = 1; /* PC=1 (physisch zusammenhängend) */
    if (nvme_admin_submit(&sqe) < 0) {
        KERR("NVMe: CREATE_CQ fehlgeschlagen\n"); return false;
    }

    /* I/O-Submission-Queue erstellen (Admin-Befehl 0x01) */
    memset(&sqe, 0, sizeof(sqe));
    sqe.cdw0  = NVME_ADMIN_CREATE_SQ;
    sqe.prp1  = (uint64_t)(uint32_t)io_sq;
    sqe.cdw10 = ((NVME_IO_QUEUE_SIZE - 1) << 16) | 1; /* QID=1 */
    sqe.cdw11 = (1u << 16) | 1; /* CQID=1, PC=1 */
    if (nvme_admin_submit(&sqe) < 0) {
        KERR("NVMe: CREATE_SQ fehlgeschlagen\n"); return false;
    }

    /* Identify Controller (um Seriennummer etc. zu lesen) */
    uint8_t *id_buf = (uint8_t *)kmalloc(4096);
    if (id_buf) {
        memset(&sqe, 0, sizeof(sqe));
        sqe.cdw0  = NVME_ADMIN_IDENTIFY;
        sqe.prp1  = (uint64_t)(uint32_t)id_buf;
        sqe.cdw10 = 1; /* CNS=1 (Controller) */
        if (nvme_admin_submit(&sqe) == 0) {
            /* Seriennummer bei Offset 4 (20 Byte) */
            char sn[21]; memcpy(sn, id_buf + 4, 20); sn[20] = '\0';
            KINFO("NVMe: SN='%.20s'\n", sn);
        }

        /* Identify Namespace 1 für Sektorzahl */
        memset(&sqe, 0, sizeof(sqe));
        sqe.cdw0  = NVME_ADMIN_IDENTIFY;
        sqe.nsid  = 1;
        sqe.prp1  = (uint64_t)(uint32_t)id_buf;
        sqe.cdw10 = 0; /* CNS=0 (Namespace) */
        if (nvme_admin_submit(&sqe) == 0) {
            /* NSZE (Namespace Size) bei Byte 0 */
            nvme_sectors = *(uint64_t *)(void *)id_buf;
            /* LBA-Format: Bits 3:0 des aktuellen Format-Index */
            uint8_t  lbaf_idx = id_buf[26] & 0x0F;
            uint32_t lbads    = (uint32_t)(id_buf[128 + lbaf_idx * 4 + 1]);
            nvme_sector_sz    = (lbads >= 9) ? (1u << lbads) : 512;
        }
        kfree(id_buf);
    }

    nvme_ok = true;
    KINFO("NVMe: bereit  Sektoren=%llu  Sektorgröße=%u B\n",
          (unsigned long long)nvme_sectors, nvme_sector_sz);
    return true;
}

/* ── Öffentlich: nvme_read ───────────────────────────────────────── */
int nvme_read(uint64_t lba, uint32_t count, void *buf)
{
    if (!nvme_ok || !buf || !count) return -1;

    nvme_sqe_t sqe;
    memset(&sqe, 0, sizeof(sqe));
    sqe.cdw0  = NVME_IO_READ;
    sqe.nsid  = 1;
    sqe.prp1  = (uint64_t)(uint32_t)buf;
    /* PRP2: zweite Seite, falls Transfer > nvme_page_size */
    sqe.prp2  = (uint64_t)(uint32_t)buf + nvme_page_size;
    sqe.cdw10 = (uint32_t)lba;
    sqe.cdw11 = (uint32_t)(lba >> 32);
    sqe.cdw12 = count - 1;   /* NLB: 0-basiert */
    return nvme_io_submit(&sqe);
}

/* ── Öffentlich: nvme_write ──────────────────────────────────────── */
int nvme_write(uint64_t lba, uint32_t count, const void *buf)
{
    if (!nvme_ok || !buf || !count) return -1;

    nvme_sqe_t sqe;
    memset(&sqe, 0, sizeof(sqe));
    sqe.cdw0  = NVME_IO_WRITE;
    sqe.nsid  = 1;
    sqe.prp1  = (uint64_t)(uint32_t)(uintptr_t)buf;
    sqe.prp2  = sqe.prp1 + nvme_page_size;
    sqe.cdw10 = (uint32_t)lba;
    sqe.cdw11 = (uint32_t)(lba >> 32);
    sqe.cdw12 = count - 1;
    return nvme_io_submit(&sqe);
}

uint32_t nvme_sector_size(void)  { return nvme_sector_sz; }
uint64_t nvme_sector_count(void) { return nvme_sectors;   }

void nvme_dump(void)
{
    if (!nvme_ok) { KINFO("NVMe: nicht initialisiert\n"); return; }
    KINFO("NVMe: %llu Sektoren × %u Byte = %llu MB\n",
          (unsigned long long)nvme_sectors,
          nvme_sector_sz,
          (unsigned long long)(nvme_sectors * nvme_sector_sz / (1024*1024)));
}