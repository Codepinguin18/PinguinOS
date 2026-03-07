/**
 * @file loopback.c
 * @brief Loopback network interface (127.0.0.1/8) for PinguinOS.
 *
 * Feature #71 – Loopback Interface
 *
 * The loopback device receives every packet it transmits by feeding
 * frames directly back into net_receive().  This allows the kernel's
 * own TCP/UDP stack to communicate with itself without physical hardware.
 */

#include "../include/net.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/heap.h"

/* ── Loopback frame buffer ───────────────────────────────────────── */
#define LO_BUF_SIZE  (ETH_MTU + ETH_HDR_LEN + 64)

static net_iface_t lo_iface;

/* ── send callback: just loop back into net_receive ─────────────── */
static int lo_send(net_iface_t *iface, const void *data, uint32_t len)
{
    if (len > LO_BUF_SIZE) return -1;

    /*
     * For loopback we skip the Ethernet framing entirely and hand the
     * raw IP packet directly to net_receive() wrapped in a minimal
     * Ethernet header so the existing dispatch code works unchanged.
     */
    uint8_t frame[LO_BUF_SIZE];
    eth_hdr_t *eth = (eth_hdr_t *)frame;

    /* Both src and dst MAC = zeroed loopback address */
    memset(&eth->dst, 0, 6);
    memset(&eth->src, 0, 6);
    eth->type = htons(ETH_TYPE_IPV4);

    if (len > LO_BUF_SIZE - ETH_HDR_LEN) len = LO_BUF_SIZE - ETH_HDR_LEN;
    memcpy(frame + ETH_HDR_LEN, data, len);

    net_receive(iface, frame, len + ETH_HDR_LEN);
    return 0;
}

/* ── Public: loopback_init ───────────────────────────────────────── */
void loopback_init(void)
{
    memset(&lo_iface, 0, sizeof(lo_iface));
    memcpy(lo_iface.name, "lo", 3);
    lo_iface.ip      = IP4(127, 0, 0, 1);
    lo_iface.netmask = IP4(255, 0, 0, 0);
    lo_iface.gateway = IP4(127, 0, 0, 1);
    lo_iface.up      = true;
    lo_iface.send    = lo_send;

    net_register_iface(&lo_iface);
    KINFO("NET: loopback interface 'lo' registered (127.0.0.1/8)\n");
}