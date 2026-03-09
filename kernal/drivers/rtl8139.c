/**
 * @file rtl8139.c
 * @brief RTL8139 10/100-Mbit-Ethernet-Treiber für PinguinOS – Feature #45.
 *
 * PCI-Gerät: Hersteller 0x10EC, Gerät 0x8139 (Realtek RTL8139).
 * Nutzt I/O-Ports (BAR0) für Register-Zugriff.
 *
 * Empfang:   Ringpuffer (8 KB + 1500 Byte Puffer)
 * Senden:    4 TX-Deskriptoren im Round-Robin-Verfahren
 *
 * Bugfixes gegenüber der alten Version:
 *   1. cpu.h eingebunden (inb/outb/inw/outw/inl/outl)
 *   2. net_receive() mit korrekten drei Argumenten (iface, data, len)
 *   3. dev->irq_line → dev->int_line (korrektes Strukturmember)
 *   4. pci_enable_bus_master() über pci.h deklariert
 *   5. net_register_nic() → net_register_iface() mit net_iface_t
 */

#include "../include/rtl8139.h"
#include "../include/pci.h"
#include "../include/idt.h"
#include "../include/net.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"   /* Fix #1: inb/outb/inw/outw/inl/outl */

/* ── PCI-Kennung ─────────────────────────────────────────────── */
#define RTL_VENDOR  0x10EC
#define RTL_DEVICE  0x8139

/* ── I/O-Register-Offsets ────────────────────────────────────── */
#define RTL_IDR0        0x00            /* MAC-Adresse (6 Byte)       */
#define RTL_MAR0        0x08            /* Multicast-Filter           */
#define RTL_TXSTATUS(n) (0x10 + (n)*4) /* TX-Status Deskriptor n     */
#define RTL_TXADDR(n)   (0x20 + (n)*4) /* TX-Pufferadresse n         */
#define RTL_RXBUF       0x30            /* RX-Ringpuffer-Adresse      */
#define RTL_CMD         0x37            /* Kommandoregister           */
#define RTL_CAPR        0x38            /* Aktuelle RX-Puffer-Adresse */
#define RTL_IMR         0x3C            /* Interrupt-Maske            */
#define RTL_ISR         0x3E            /* Interrupt-Status           */
#define RTL_TCR         0x40            /* TX-Konfiguration           */
#define RTL_RCR         0x44            /* RX-Konfiguration           */
#define RTL_CONFIG1     0x52            /* Konfiguration 1            */

/* ── Bit-Definitionen ────────────────────────────────────────── */
#define RTL_CMD_TE   0x04   /* TX aktivieren */
#define RTL_CMD_RE   0x08   /* RX aktivieren */
#define RTL_CMD_RST  0x10   /* Reset         */

#define RTL_IMR_ROK  0x01   /* Empfang OK     */
#define RTL_IMR_TOK  0x04   /* Senden OK      */
#define RTL_IMR_RER  0x02   /* Empfangsfehler */

#define RTL_RCR_AAP  (1u << 0)   /* Alle physischen Pakete       */
#define RTL_RCR_APM  (1u << 1)   /* Physische Pakete an uns      */
#define RTL_RCR_AM   (1u << 2)   /* Multicast                    */
#define RTL_RCR_AB   (1u << 3)   /* Broadcast                    */
#define RTL_RCR_WRAP (1u << 7)   /* Ringpuffer Wrap-Around       */
#define RTL_RCR_RBLEN_8K (0u << 11)

/* ── Puffergrößen ────────────────────────────────────────────── */
#define RTL_RX_BUF_SIZE   (8192 + 16 + 1500)
#define RTL_TX_BUFS       4
#define RTL_TX_BUF_SIZE   1536

/* ── Modul-Zustand ───────────────────────────────────────────── */
static uint16_t  io_base   = 0;
static uint8_t  *rx_buf    = NULL;
static uint8_t  *tx_buf[RTL_TX_BUFS];
static uint32_t  tx_cur    = 0;   /* Nächster TX-Deskriptor   */
static uint32_t  rx_pos    = 0;   /* Aktuelle RX-Leseposition */
static uint8_t   rtl_mac[6];

/* Fix #5: net_iface_t für die Registrierung beim Netzwerk-Stack */
static net_iface_t rtl8139_iface;

/* ── I/O-Hilfsfunktionen ─────────────────────────────────────── */
static INLINE uint8_t  rtl_inb(uint16_t reg) { return inb((uint16_t)(io_base + reg)); }
static INLINE uint16_t rtl_inw(uint16_t reg) { return inw((uint16_t)(io_base + reg)); }
static INLINE uint32_t rtl_inl(uint16_t reg) { return inl((uint16_t)(io_base + reg)); }
static INLINE void rtl_outb(uint16_t reg, uint8_t  v) { outb((uint16_t)(io_base + reg), v); }
static INLINE void rtl_outw(uint16_t reg, uint16_t v) { outw((uint16_t)(io_base + reg), v); }
static INLINE void rtl_outl(uint16_t reg, uint32_t v) { outl((uint16_t)(io_base + reg), v); }

/* ── IRQ-Handler ─────────────────────────────────────────────── */
static void rtl_irq_handler(cpu_regs_t *regs)
{
    (void)regs;
    uint16_t isr = rtl_inw(RTL_ISR);
    rtl_outw(RTL_ISR, isr);   /* Interrupts quittieren */

    /* Empfangene Pakete verarbeiten (solange RX-Puffer nicht leer) */
    while (!(rtl_inb(RTL_CMD) & 0x01)) {
        uint32_t offset = rx_pos % (8192 + 16);
        uint8_t *hdr    = rx_buf + offset;

        /* RTL8139-Paketkopf: Status (2 Byte) + Länge (2 Byte) */
        uint16_t pkt_status = *(uint16_t *)(void *)(hdr);
        uint16_t pkt_len    = *(uint16_t *)(void *)(hdr + 2);

        /* Fehlerhafte oder zu große Pakete überspringen */
        if (!(pkt_status & 0x01) || pkt_len > 1514 || pkt_len < 4) {
            rx_pos = (rx_pos + pkt_len + 4 + 3) & ~3u;
            rtl_outw(RTL_CAPR, (uint16_t)(rx_pos - 16));
            continue;
        }

        /* Fix #2: Paket an den Netzwerk-Stack übergeben – korrekte Signatur:
         * net_receive(net_iface_t *iface, const void *data, uint32_t len) */
        net_receive(&rtl8139_iface, hdr + 4, (uint32_t)(pkt_len - 4));

        /* Lesezeiger vorrücken (4-Byte-ausgerichtet) */
        rx_pos = (rx_pos + pkt_len + 4 + 3) & ~3u;
        rtl_outw(RTL_CAPR, (uint16_t)(rx_pos - 16));
    }
}

/* ── Sende-Funktion im Format der net_iface_t ────────────────── */
static int rtl8139_send_frame(net_iface_t *iface, const void *data, uint32_t len)
{
    (void)iface;
    if (!io_base || len > RTL_TX_BUF_SIZE) return -1;

    /* Daten in TX-Puffer kopieren */
    memcpy(tx_buf[tx_cur], data, len);

    /* Übertragung starten: Länge in TX-Status schreiben */
    rtl_outl(RTL_TXSTATUS(tx_cur), len);

    /* Nächsten TX-Deskriptor wählen */
    tx_cur = (tx_cur + 1) % RTL_TX_BUFS;
    return 0;
}

/* ── Öffentlich: rtl8139_init ────────────────────────────────── */
int rtl8139_init(void)
{
    /* PCI-Gerät suchen */
    pci_device_t *dev = pci_find_device(RTL_VENDOR, RTL_DEVICE);
    if (!dev) return -1;

    /* Fix #4: Bus-Mastering aktivieren (jetzt korrekt deklariert) */
    pci_enable_bus_master(dev);

    /* I/O-Basisadresse aus BAR0 (Bit 0 = I/O-Typ-Flag, Bit 1 reserviert) */
    io_base = (uint16_t)(dev->bar[0] & ~0x3u);
    if (!io_base) return -1;

    /* Software-Reset durchführen */
    rtl_outb(RTL_CMD, RTL_CMD_RST);
    uint32_t timeout = 100000;
    while ((rtl_inb(RTL_CMD) & RTL_CMD_RST) && timeout--);
    if (!timeout) { KERR("RTL8139: Reset-Timeout\n"); return -1; }

    /* MAC-Adresse aus den IDR-Registern lesen */
    for (int i = 0; i < 6; i++) rtl_mac[i] = rtl_inb((uint16_t)(RTL_IDR0 + i));
    KINFO("RTL8139: MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
          rtl_mac[0], rtl_mac[1], rtl_mac[2],
          rtl_mac[3], rtl_mac[4], rtl_mac[5]);

    /* RX-Ringpuffer allozieren und konfigurieren */
    rx_buf = (uint8_t *)kmalloc(RTL_RX_BUF_SIZE);
    if (!rx_buf) return -1;
    memset(rx_buf, 0, RTL_RX_BUF_SIZE);
    rtl_outl(RTL_RXBUF, (uint32_t)rx_buf);

    /* TX-Puffer allozieren und Adressen im NIC hinterlegen */
    for (int i = 0; i < RTL_TX_BUFS; i++) {
        tx_buf[i] = (uint8_t *)kmalloc(RTL_TX_BUF_SIZE);
        if (!tx_buf[i]) return -1;
        rtl_outl(RTL_TXADDR(i), (uint32_t)tx_buf[i]);
    }

    /* Fix #3: IRQ registrieren – int_line statt irq_line */
    if (dev->int_line < 16)
        irq_register(dev->int_line, rtl_irq_handler);

    /* Interrupts aktivieren: Empfang-OK, Empfangsfehler, Sende-OK */
    rtl_outw(RTL_IMR, RTL_IMR_ROK | RTL_IMR_RER | RTL_IMR_TOK);

    /* RX-Konfiguration: alle Pakettypen empfangen, 8-KB-Ringpuffer */
    rtl_outl(RTL_RCR, RTL_RCR_AAP | RTL_RCR_APM | RTL_RCR_AM |
                      RTL_RCR_AB  | RTL_RCR_WRAP | RTL_RCR_RBLEN_8K);

    /* TX-Konfiguration: Standard-Einstellungen */
    rtl_outl(RTL_TCR, 0x03000700u);

    /* TX und RX aktivieren */
    rtl_outb(RTL_CMD, RTL_CMD_TE | RTL_CMD_RE);

    /* Fix #5: net_iface_t aufbauen und beim Netzwerk-Stack registrieren */
    memset(&rtl8139_iface, 0, sizeof(rtl8139_iface));
    memcpy(rtl8139_iface.name, "eth1", 5);
    memcpy(rtl8139_iface.mac.b, rtl_mac, 6);
    rtl8139_iface.send = rtl8139_send_frame;
    rtl8139_iface.up   = true;
    net_register_iface(&rtl8139_iface);

    KINFO("RTL8139: bereit  IO=0x%04x  IRQ=%u\n", io_base, dev->int_line);
    return 0;
}

/* ── Öffentlich: rtl8139_send (Wrapper für Abwärtskompatibilität) */
int rtl8139_send(const uint8_t *data, uint16_t len)
{
    return rtl8139_send_frame(&rtl8139_iface, data, (uint32_t)len);
}

/** MAC-Adresse in buf (6 Byte) kopieren. */
void rtl8139_get_mac(uint8_t *buf)
{
    memcpy(buf, rtl_mac, 6);
}