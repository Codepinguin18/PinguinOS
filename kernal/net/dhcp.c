/**
 * @file dhcp.c
 * @brief DHCP client (RFC 2131) for PinguinOS.
 *
 * Feature #69 – DHCP Client
 *
 * Implements the four-step DORA exchange:
 *   1. DHCPDISCOVER – broadcast on 255.255.255.255:67
 *   2. DHCPOFFER    – server replies with an offered IP
 *   3. DHCPREQUEST  – client requests the offered IP
 *   4. DHCPACK      – server confirms; we apply the lease
 *
 * After a successful lease dhcp_configure_iface() writes the obtained
 * IP, netmask, gateway and DNS into the net_iface_t.
 *
 * This is a minimal blocking implementation (poll-based, no retries).
 * A full production implementation would use timers and retransmission.
 */

#include "../include/net.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"   /* task_sleep() */

/* ── BOOTP / DHCP constants ──────────────────────────────────────── */
#define DHCP_SERVER_PORT  67
#define DHCP_CLIENT_PORT  68
#define DHCP_MAGIC_COOKIE 0x63825363u

#define DHCP_OP_REQUEST   1
#define DHCP_OP_REPLY     2
#define DHCP_HTYPE_ETH    1
#define DHCP_HLEN_ETH     6

/* Option codes */
#define DHCP_OPT_SUBNET_MASK   1
#define DHCP_OPT_ROUTER        3
#define DHCP_OPT_DNS           6
#define DHCP_OPT_HOSTNAME      12
#define DHCP_OPT_LEASE_TIME    51
#define DHCP_OPT_MSG_TYPE      53
#define DHCP_OPT_SERVER_ID     54
#define DHCP_OPT_PARAM_REQ     55
#define DHCP_OPT_END           255

/* Message type values (option 53) */
#define DHCP_MSG_DISCOVER  1
#define DHCP_MSG_OFFER     2
#define DHCP_MSG_REQUEST   3
#define DHCP_MSG_DECLINE   4
#define DHCP_MSG_ACK       5
#define DHCP_MSG_NAK       6
#define DHCP_MSG_RELEASE   7

/* ── DHCP packet structure (BOOTP fixed header = 236 bytes) ──────── */
#define DHCP_OPTIONS_LEN  312

typedef struct __attribute__((packed)) {
    uint8_t  op;            /* 1=request, 2=reply              */
    uint8_t  htype;         /* 1=Ethernet                      */
    uint8_t  hlen;          /* 6 for MAC                       */
    uint8_t  hops;
    uint32_t xid;           /* Transaction ID                  */
    uint16_t secs;
    uint16_t flags;         /* 0x8000 = broadcast              */
    uint32_t ciaddr;        /* Client IP (0 if unknown)        */
    uint32_t yiaddr;        /* "Your" IP (from server)         */
    uint32_t siaddr;        /* Server IP                       */
    uint32_t giaddr;        /* Gateway/relay IP                */
    uint8_t  chaddr[16];    /* Client MAC (padded)             */
    uint8_t  sname[64];     /* Server hostname                 */
    uint8_t  file[128];     /* Boot filename                   */
    uint32_t magic;         /* DHCP magic cookie               */
    uint8_t  options[DHCP_OPTIONS_LEN];
} dhcp_pkt_t;

/* ── Lease information returned to caller ────────────────────────── */
typedef struct {
    uint32_t  ip;
    uint32_t  netmask;
    uint32_t  gateway;
    uint32_t  dns;
    uint32_t  server_id;
    uint32_t  lease_secs;
} dhcp_lease_t;

/* ── Module state ────────────────────────────────────────────────── */
static dhcp_lease_t current_lease;
static bool         lease_valid = false;

/* ── Helpers ─────────────────────────────────────────────────────── */
static uint32_t g_xid = 0xDEAD1234u;

/** Build a DHCP DISCOVER or REQUEST packet. */
static void dhcp_build_pkt(dhcp_pkt_t *pkt, const uint8_t *mac,
                            uint8_t msg_type, uint32_t server_id,
                            uint32_t offered_ip)
{
    memset(pkt, 0, sizeof(*pkt));
    pkt->op    = DHCP_OP_REQUEST;
    pkt->htype = DHCP_HTYPE_ETH;
    pkt->hlen  = DHCP_HLEN_ETH;
    pkt->xid   = htonl(g_xid);
    pkt->flags = htons(0x8000);   /* Broadcast flag */
    pkt->magic = htonl(DHCP_MAGIC_COOKIE);
    memcpy(pkt->chaddr, mac, 6);

    uint8_t *opt = pkt->options;
    /* Option 53: Message type */
    *opt++ = DHCP_OPT_MSG_TYPE; *opt++ = 1; *opt++ = msg_type;

    if (msg_type == DHCP_MSG_REQUEST) {
        /* Option 54: Server identifier */
        *opt++ = DHCP_OPT_SERVER_ID; *opt++ = 4;
        uint32_t si_net = htonl(server_id);
        memcpy(opt, &si_net, 4); opt += 4;

        /* Option 50: Requested IP */
        *opt++ = 50; *opt++ = 4;
        uint32_t ip_net = htonl(offered_ip);
        memcpy(opt, &ip_net, 4); opt += 4;
    }

    /* Option 55: Parameter request list */
    *opt++ = DHCP_OPT_PARAM_REQ;
    *opt++ = 4;
    *opt++ = DHCP_OPT_SUBNET_MASK;
    *opt++ = DHCP_OPT_ROUTER;
    *opt++ = DHCP_OPT_DNS;
    *opt++ = DHCP_OPT_LEASE_TIME;

    *opt++ = DHCP_OPT_END;
}

/** Parse options from a DHCP reply packet into a lease struct. */
static bool dhcp_parse_reply(const dhcp_pkt_t *pkt, uint8_t expected_type,
                              dhcp_lease_t *lease)
{
    if (ntohl(pkt->magic) != DHCP_MAGIC_COOKIE) return false;

    lease->ip = ntohl(pkt->yiaddr);

    const uint8_t *opt = pkt->options;
    const uint8_t *end = pkt->options + DHCP_OPTIONS_LEN;
    uint8_t msg_type = 0;

    while (opt < end && *opt != DHCP_OPT_END) {
        uint8_t code = *opt++;
        if (code == 0) continue;   /* Pad */
        if (opt >= end) break;
        uint8_t len = *opt++;
        if (opt + len > end) break;

        switch (code) {
        case DHCP_OPT_MSG_TYPE:
            if (len >= 1) msg_type = *opt;
            break;
        case DHCP_OPT_SUBNET_MASK:
            if (len == 4) {
                uint32_t v; memcpy(&v, opt, 4);
                lease->netmask = ntohl(v);
            }
            break;
        case DHCP_OPT_ROUTER:
            if (len >= 4) {
                uint32_t v; memcpy(&v, opt, 4);
                lease->gateway = ntohl(v);
            }
            break;
        case DHCP_OPT_DNS:
            if (len >= 4) {
                uint32_t v; memcpy(&v, opt, 4);
                lease->dns = ntohl(v);
            }
            break;
        case DHCP_OPT_SERVER_ID:
            if (len == 4) {
                uint32_t v; memcpy(&v, opt, 4);
                lease->server_id = ntohl(v);
            }
            break;
        case DHCP_OPT_LEASE_TIME:
            if (len == 4) {
                uint32_t v; memcpy(&v, opt, 4);
                lease->lease_secs = ntohl(v);
            }
            break;
        }
        opt += len;
    }

    return msg_type == expected_type;
}

/* ── Public: dhcp_discover ───────────────────────────────────────── */
/**
 * @brief Perform a full DORA exchange on the given interface.
 *
 * Sends DISCOVER, waits up to @p timeout_ms for an OFFER, then sends
 * REQUEST and waits for ACK.  On success the interface's IP / mask /
 * gateway fields are updated and true is returned.
 */
bool dhcp_discover(net_iface_t *iface, uint32_t timeout_ms)
{
    if (!iface) return false;

    /* Open a UDP socket on port 68 */
    int sock = sock_open(IP_PROTO_UDP);
    if (sock < 0) {
        KERR("DHCP: cannot open UDP socket\n");
        return false;
    }
    sock_bind(sock, DHCP_CLIENT_PORT);

    dhcp_pkt_t pkt;
    dhcp_lease_t offer;
    memset(&offer, 0, sizeof(offer));

    /* ── Step 1: DISCOVER ─────────────────────────────────────────── */
    dhcp_build_pkt(&pkt, iface->mac.b, DHCP_MSG_DISCOVER, 0, 0);
    sock_sendto(sock, 0xFFFFFFFFu, DHCP_SERVER_PORT,
               &pkt, sizeof(pkt));
    KINFO("DHCP: DISCOVER sent  (xid=0x%08x)\n", g_xid);

    /* ── Step 2: Wait for OFFER ──────────────────────────────────── */
    uint8_t  rx_buf[sizeof(dhcp_pkt_t)];
    uint32_t elapsed = 0;
    bool     got_offer = false;

    while (elapsed < timeout_ms) {
        uint32_t from_ip;
        uint16_t from_port;
        int n = sock_recvfrom(sock, rx_buf, sizeof(rx_buf),
                             &from_ip, &from_port);
        if (n >= (int)sizeof(dhcp_pkt_t)) {
            const dhcp_pkt_t *reply = (const dhcp_pkt_t *)rx_buf;
            if (ntohl(reply->xid) == g_xid &&
                dhcp_parse_reply(reply, DHCP_MSG_OFFER, &offer)) {
                got_offer = true;
                break;
            }
        }
        task_sleep(10);
        elapsed += 10;
    }

    if (!got_offer) {
        KERR("DHCP: no OFFER received (timeout %u ms)\n", timeout_ms);
        sock_close(sock);
        return false;
    }

    KINFO("DHCP: OFFER  IP=%u.%u.%u.%u  server=%u.%u.%u.%u\n",
          (offer.ip >> 24) & 0xFF, (offer.ip >> 16) & 0xFF,
          (offer.ip >>  8) & 0xFF,  offer.ip        & 0xFF,
          (offer.server_id >> 24) & 0xFF, (offer.server_id >> 16) & 0xFF,
          (offer.server_id >>  8) & 0xFF,  offer.server_id        & 0xFF);

    /* ── Step 3: REQUEST ──────────────────────────────────────────── */
    dhcp_build_pkt(&pkt, iface->mac.b, DHCP_MSG_REQUEST,
                   offer.server_id, offer.ip);
    sock_sendto(sock, 0xFFFFFFFFu, DHCP_SERVER_PORT,
               &pkt, sizeof(pkt));
    KINFO("DHCP: REQUEST sent\n");

    /* ── Step 4: Wait for ACK ────────────────────────────────────── */
    dhcp_lease_t ack;
    memset(&ack, 0, sizeof(ack));
    bool got_ack = false;
    elapsed = 0;

    while (elapsed < timeout_ms) {
        uint32_t from_ip;
        uint16_t from_port;
        int n = sock_recvfrom(sock, rx_buf, sizeof(rx_buf),
                             &from_ip, &from_port);
        if (n >= (int)sizeof(dhcp_pkt_t)) {
            const dhcp_pkt_t *reply = (const dhcp_pkt_t *)rx_buf;
            if (ntohl(reply->xid) == g_xid &&
                dhcp_parse_reply(reply, DHCP_MSG_ACK, &ack)) {
                got_ack = true;
                break;
            }
        }
        task_sleep(10);
        elapsed += 10;
    }

    sock_close(sock);

    if (!got_ack) {
        KERR("DHCP: no ACK received\n");
        return false;
    }

    /* ── Apply lease ──────────────────────────────────────────────── */
    current_lease = ack;
    lease_valid   = true;
    g_xid++;

    iface->ip      = ack.ip;
    iface->netmask = ack.netmask;
    iface->gateway = ack.gateway;

    KINFO("DHCP: ACK  IP=%u.%u.%u.%u  mask=%u.%u.%u.%u  gw=%u.%u.%u.%u  "
          "lease=%u s\n",
          (ack.ip >> 24) & 0xFF,      (ack.ip >> 16) & 0xFF,
          (ack.ip >>  8) & 0xFF,       ack.ip        & 0xFF,
          (ack.netmask >> 24) & 0xFF, (ack.netmask >> 16) & 0xFF,
          (ack.netmask >>  8) & 0xFF,  ack.netmask        & 0xFF,
          (ack.gateway >> 24) & 0xFF, (ack.gateway >> 16) & 0xFF,
          (ack.gateway >>  8) & 0xFF,  ack.gateway        & 0xFF,
          ack.lease_secs);

    return true;
}

/* ── Public: dhcp_get_lease ──────────────────────────────────────── */
bool dhcp_get_lease(uint32_t *ip, uint32_t *netmask,
                    uint32_t *gateway, uint32_t *dns)
{
    if (!lease_valid) return false;
    if (ip)      *ip      = current_lease.ip;
    if (netmask) *netmask = current_lease.netmask;
    if (gateway) *gateway = current_lease.gateway;
    if (dns)     *dns     = current_lease.dns;
    return true;
}