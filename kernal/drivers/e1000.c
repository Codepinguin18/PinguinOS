/**
 * @file e1000.c
 * @brief Intel E1000 (82540EM) NIC driver for PinguinOS.
 *
 * Feature #44 – E1000 NIC
 *
 * Implementation covers:
 *   – PCI detection (vendor 0x8086, device 0x100E)
 *   – MMIO BAR0 mapping
 *   – EEPROM MAC address read
 *   – RX / TX descriptor ring setup (8 descriptors each)
 *   – IRQ handler for packet reception
 *   – net_iface_t registration
 *
 * Descriptor ring sizes are intentionally small (8 each) to keep BSS
 * usage modest.  A production driver would use 256+.
 */

#include "../include/e1000.h"
#include "../include/pci.h"
#include "../include/net.h"
#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/heap.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── E1000 PCI IDs ───────────────────────────────────────────────── */
#define E1000_VENDOR  0x8086
#define E1000_DEVICE  0x100E

/* ── E1000 Register offsets ──────────────────────────────────────── */
#define E1000_REG_CTRL      0x0000
#define E1000_REG_STATUS    0x0008
#define E1000_REG_EECD      0x0010   /* EEPROM/Flash Control            */
#define E1000_REG_EERD      0x0014   /* EEPROM Read                     */
#define E1000_REG_ICR       0x00C0   /* Interrupt Cause Read            */
#define E1000_REG_ITR       0x00C4   /* Interrupt Throttling            */
#define E1000_REG_IMS       0x00D0   /* Interrupt Mask Set              */
#define E1000_REG_IMC       0x00D8   /* Interrupt Mask Clear            */
#define E1000_REG_RCTL      0x0100   /* Receive Control                 */
#define E1000_REG_TCTL      0x0400   /* Transmit Control                */
#define E1000_REG_TIPG      0x0410   /* TX Inter-Packet Gap             */
#define E1000_REG_RDBAL     0x2800   /* RX Desc Base Addr Low           */
#define E1000_REG_RDBAH     0x2804   /* RX Desc Base Addr High          */
#define E1000_REG_RDLEN     0x2808   /* RX Desc Ring Length             */
#define E1000_REG_RDH       0x2810   /* RX Desc Head                    */
#define E1000_REG_RDT       0x2818   /* RX Desc Tail                    */
#define E1000_REG_TDBAL     0x3800   /* TX Desc Base Addr Low           */
#define E1000_REG_TDBAH     0x3804   /* TX Desc Base Addr High          */
#define E1000_REG_TDLEN     0x3808   /* TX Desc Ring Length             */
#define E1000_REG_TDH       0x3810   /* TX Desc Head                    */
#define E1000_REG_TDT       0x3818   /* TX Desc Tail                    */
#define E1000_REG_RAL       0x5400   /* Receive Address Low (0)         */
#define E1000_REG_RAH       0x5404   /* Receive Address High (0)        */
#define E1000_REG_MTA       0x5200   /* Multicast Table Array (128 DW)  */

/* CTRL bits */
#define E1000_CTRL_RST      (1u << 26)   /* Full reset */
#define E1000_CTRL_SLU      (1u << 6)    /* Set Link Up */
#define E1000_CTRL_ASDE     (1u << 5)    /* Auto-Speed Detection */

/* RCTL bits */
#define E1000_RCTL_EN       (1u << 1)
#define E1000_RCTL_BAM      (1u << 15)   /* Broadcast Accept Mode */
#define E1000_RCTL_BSIZE_2K (0u << 16)
#define E1000_RCTL_SECRC    (1u << 26)   /* Strip CRC */

/* TCTL bits */
#define E1000_TCTL_EN       (1u << 1)
#define E1000_TCTL_PSP      (1u << 3)    /* Pad Short Packets */
#define E1000_TCTL_CT_SHIFT 4
#define E1000_TCTL_COLD_SHIFT 12

/* Interrupt bits (ICR/IMS) */
#define E1000_ICR_TXDW      (1u << 0)    /* TX descriptor written back */
#define E1000_ICR_RXT0      (1u << 7)    /* RX timer interrupt         */
#define E1000_ICR_LSC       (1u << 2)    /* Link Status Change         */

/* EERD bits */
#define E1000_EERD_START    (1u << 0)
#define E1000_EERD_DONE     (1u << 4)

/* Descriptor status bits */
#define E1000_RXD_STA_DD    (1u << 0)    /* Descriptor Done */
#define E1000_RXD_STA_EOP   (1u << 1)    /* End of Packet  */
#define E1000_TXD_STA_DD    (1u << 0)
#define E1000_TXD_CMD_EOP   (1u << 0)
#define E1000_TXD_CMD_IFCS  (1u << 1)    /* Insert FCS */
#define E1000_TXD_CMD_RS    (1u << 3)    /* Report Status */

/* ── Descriptor types ────────────────────────────────────────────── */
#define E1000_NUM_RX_DESC   8
#define E1000_NUM_TX_DESC   8
#define E1000_BUF_SIZE      2048

typedef struct __attribute__((packed)) {
    uint64_t addr;
    uint16_t length;
    uint16_t checksum;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
} e1000_rx_desc_t;

typedef struct __attribute__((packed)) {
    uint64_t addr;
    uint16_t length;
    uint8_t  cso;
    uint8_t  cmd;
    uint8_t  status;
    uint8_t  css;
    uint16_t special;
} e1000_tx_desc_t;

/* ── Module state ────────────────────────────────────────────────── */
static volatile uint32_t *e1000_mmio = NULL;   /* MMIO base */
static uint8_t  e1000_mac[6];

static e1000_rx_desc_t rx_descs[E1000_NUM_RX_DESC] __attribute__((aligned(16)));
static e1000_tx_desc_t tx_descs[E1000_NUM_TX_DESC] __attribute__((aligned(16)));
static uint8_t  rx_buffers[E1000_NUM_RX_DESC][E1000_BUF_SIZE];
static uint8_t  tx_buffers[E1000_NUM_TX_DESC][E1000_BUF_SIZE];

static uint32_t rx_tail = 0;
static uint32_t tx_tail = 0;

static net_iface_t e1000_iface;
static bool        e1000_present = false;

/* ── Register helpers ────────────────────────────────────────────── */
static inline uint32_t e1000_read(uint32_t reg)
{
    return e1000_mmio[reg / 4];
}

static inline void e1000_write(uint32_t reg, uint32_t val)
{
    e1000_mmio[reg / 4] = val;
}

/* ── EEPROM read ─────────────────────────────────────────────────── */
static uint16_t e1000_eeprom_read(uint8_t addr)
{
    e1000_write(E1000_REG_EERD, (uint32_t)(addr << 8) | E1000_EERD_START);
    uint32_t val;
    uint32_t timeout = 1000000;
    do {
        val = e1000_read(E1000_REG_EERD);
    } while (!(val & E1000_EERD_DONE) && --timeout);
    return (uint16_t)(val >> 16);
}

/* ── IRQ handler ─────────────────────────────────────────────────── */
static void e1000_irq_handler(cpu_regs_t *regs)
{
    (void)regs;
    uint32_t icr = e1000_read(E1000_REG_ICR);   /* Reading clears bits */

    if (icr & E1000_ICR_LSC) {
        KINFO("E1000: Link status change\n");
    }

    if (icr & E1000_ICR_RXT0) {
        /* Process received packets */
        while (rx_descs[rx_tail].status & E1000_RXD_STA_DD) {
            uint16_t len    = rx_descs[rx_tail].length;
            uint8_t *packet = rx_buffers[rx_tail];

            /* Hand up to the network stack */
            net_receive(&e1000_iface, packet, len);

            /* Reset descriptor */
            rx_descs[rx_tail].status = 0;
            e1000_write(E1000_REG_RDT, rx_tail);
            rx_tail = (rx_tail + 1) % E1000_NUM_RX_DESC;
        }
    }
}

/* ── Transmit function (called by net_iface_t.send) ─────────────── */
static int e1000_send_frame(net_iface_t *iface, const void *data, uint32_t len)
{
    (void)iface;
    if (!e1000_present || len > E1000_BUF_SIZE) return -1;

    memcpy(tx_buffers[tx_tail], data, len);

    tx_descs[tx_tail].addr   = (uint64_t)(uint32_t)tx_buffers[tx_tail];
    tx_descs[tx_tail].length = len;
    tx_descs[tx_tail].cmd    = E1000_TXD_CMD_EOP | E1000_TXD_CMD_IFCS |
                               E1000_TXD_CMD_RS;
    tx_descs[tx_tail].status = 0;

    tx_tail = (tx_tail + 1) % E1000_NUM_TX_DESC;
    e1000_write(E1000_REG_TDT, tx_tail);

    return 0;
}

/* ── Public: e1000_init ──────────────────────────────────────────── */
bool e1000_init(void)
{
    pci_device_t *dev = pci_find_device(E1000_VENDOR, E1000_DEVICE);
    if (!dev) {
        KINFO("E1000: not found\n");
        return false;
    }

    /* Map MMIO BAR0 */
    uint32_t mmio_phys = dev->bar[0] & ~0xFu;
    uint32_t mmio_size = 0x20000;   /* 128 KB */
    for (uint32_t i = 0; i < mmio_size; i += PAGE_SIZE) {
        paging_map(mmio_phys + i, mmio_phys + i,
                   PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);
    }
    e1000_mmio = (volatile uint32_t *)mmio_phys;

    /* Full reset */
    e1000_write(E1000_REG_CTRL, e1000_read(E1000_REG_CTRL) | E1000_CTRL_RST);
    /* Wait for reset to clear (hardware clears the RST bit) */
    uint32_t timeout = 1000000;
    while ((e1000_read(E1000_REG_CTRL) & E1000_CTRL_RST) && --timeout);

    /* Auto-speed + set link up */
    e1000_write(E1000_REG_CTRL,
                e1000_read(E1000_REG_CTRL) | E1000_CTRL_SLU | E1000_CTRL_ASDE);

    /* Read MAC from EEPROM */
    uint16_t w0 = e1000_eeprom_read(0);
    uint16_t w1 = e1000_eeprom_read(1);
    uint16_t w2 = e1000_eeprom_read(2);
    e1000_mac[0] = w0 & 0xFF; e1000_mac[1] = (w0 >> 8) & 0xFF;
    e1000_mac[2] = w1 & 0xFF; e1000_mac[3] = (w1 >> 8) & 0xFF;
    e1000_mac[4] = w2 & 0xFF; e1000_mac[5] = (w2 >> 8) & 0xFF;

    KINFO("E1000: MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
          e1000_mac[0], e1000_mac[1], e1000_mac[2],
          e1000_mac[3], e1000_mac[4], e1000_mac[5]);

    /* Clear MTA */
    for (int i = 0; i < 128; i++)
        e1000_write(E1000_REG_MTA + i * 4, 0);

    /* Set up RX descriptors */
    for (int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rx_descs[i].addr   = (uint64_t)(uint32_t)rx_buffers[i];
        rx_descs[i].status = 0;
    }
    e1000_write(E1000_REG_RDBAL, (uint32_t)rx_descs);
    e1000_write(E1000_REG_RDBAH, 0);
    e1000_write(E1000_REG_RDLEN, sizeof(rx_descs));
    e1000_write(E1000_REG_RDH,   0);
    e1000_write(E1000_REG_RDT,   E1000_NUM_RX_DESC - 1);
    e1000_write(E1000_REG_RCTL,
                E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_SECRC);

    /* Set up TX descriptors */
    memset(tx_descs, 0, sizeof(tx_descs));
    e1000_write(E1000_REG_TDBAL, (uint32_t)tx_descs);
    e1000_write(E1000_REG_TDBAH, 0);
    e1000_write(E1000_REG_TDLEN, sizeof(tx_descs));
    e1000_write(E1000_REG_TDH,   0);
    e1000_write(E1000_REG_TDT,   0);
    e1000_write(E1000_REG_TCTL,
                E1000_TCTL_EN | E1000_TCTL_PSP |
                (15u << E1000_TCTL_CT_SHIFT) |
                (63u << E1000_TCTL_COLD_SHIFT));
    e1000_write(E1000_REG_TIPG,  0x0060200A);

    /* Enable RX and link-change interrupts */
    e1000_write(E1000_REG_IMS, E1000_ICR_RXT0 | E1000_ICR_LSC);

    /* Register IRQ handler */
    if (dev->int_line < 16)
        irq_register(dev->int_line, e1000_irq_handler);

    /* Register with the network stack */
    memset(&e1000_iface, 0, sizeof(e1000_iface));
    memcpy(e1000_iface.name, "eth0", 5);
    memcpy(e1000_iface.mac.b, e1000_mac, 6);
    e1000_iface.send = e1000_send_frame;
    /* IP/mask/gateway configured by DHCP or statically */
    net_register_iface(&e1000_iface);

    e1000_present = true;
    KINFO("E1000: driver initialised  (IRQ%u  MMIO=0x%08x)\n",
          dev->int_line, mmio_phys);
    return true;
}

/* ── Public: e1000_send ──────────────────────────────────────────── */
bool e1000_send(const void *data, uint32_t len)
{
    return e1000_send_frame(&e1000_iface, data, len) == 0;
}

const uint8_t *e1000_get_mac(void) { return e1000_mac; }