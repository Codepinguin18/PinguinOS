/**
 * @file net.c
 * @brief Vollständiger Mini-Netzwerk-Stack für PinguinOS.
 *
 * Implementiert Features #61–75:
 *   Ethernet, ARP, IPv4, ICMP, UDP, Socket-API, Statistiken
 */
#include "../include/net.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── Globaler Zustand ────────────────────────────────────────────── */
#define MAX_IFACES  4

static net_iface_t *ifaces[MAX_IFACES];
static uint32_t     iface_count = 0;
static arp_entry_t  arp_cache[ARP_CACHE_SIZE];
static socket_t     sockets[SOCK_MAX];
static net_stats_t  stats;
static uint16_t     ip_id_counter = 1;

/* ── Broadcast-MAC ───────────────────────────────────────────────── */
static const mac_addr_t MAC_BROADCAST = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};

/* ── IP-Prüfsumme ────────────────────────────────────────────────── */
static uint16_t ip_checksum(const void *data, uint32_t len)
{
    const uint16_t *p = (const uint16_t *)data;
    uint32_t sum = 0;

    while (len > 1) { sum += *p++; len -= 2; }
    if (len) sum += *(const uint8_t *)p;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

/* ── net_init ────────────────────────────────────────────────────── */
void net_init(void)
{
    memset(ifaces,    0, sizeof(ifaces));
    memset(arp_cache, 0, sizeof(arp_cache));
    memset(sockets,   0, sizeof(sockets));
    memset(&stats,    0, sizeof(stats));
    iface_count = 0;
    KINFO("NET: Netzwerk-Stack initialisiert\n");
}

int net_register_iface(net_iface_t *iface)
{
    if (iface_count >= MAX_IFACES) return -1;
    ifaces[iface_count++] = iface;
    KINFO("NET: Interface '%s' registriert  IP=%u.%u.%u.%u\n",
          iface->name,
          (iface->ip >> 24) & 0xFF, (iface->ip >> 16) & 0xFF,
          (iface->ip >>  8) & 0xFF,  iface->ip        & 0xFF);
    return 0;
}

/* ══════════════════════════════════════════════════════════════════
 *  ARP – Features #62
 * ══════════════════════════════════════════════════════════════════ */
void arp_add(ip4_addr_t ip, const mac_addr_t *mac)
{
    /* Vorhandenen Eintrag aktualisieren */
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].valid && arp_cache[i].ip == ip) {
            arp_cache[i].mac = *mac;
            arp_cache[i].ttl = 300;
            return;
        }
    }
    /* Freien Slot suchen */
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (!arp_cache[i].valid) {
            arp_cache[i].ip    = ip;
            arp_cache[i].mac   = *mac;
            arp_cache[i].ttl   = 300;
            arp_cache[i].valid = true;
            return;
        }
    }
}

mac_addr_t *arp_lookup(ip4_addr_t ip)
{
    for (int i = 0; i < ARP_CACHE_SIZE; i++)
        if (arp_cache[i].valid && arp_cache[i].ip == ip)
            return &arp_cache[i].mac;
    return NULL;
}

void arp_send_request(net_iface_t *iface, ip4_addr_t target_ip)
{
    uint8_t pkt[sizeof(eth_hdr_t) + sizeof(arp_pkt_t)];
    memset(pkt, 0, sizeof(pkt));

    eth_hdr_t *eth = (eth_hdr_t *)pkt;
    arp_pkt_t *arp = (arp_pkt_t *)(pkt + sizeof(eth_hdr_t));

    eth->dst  = MAC_BROADCAST;
    eth->src  = iface->mac;
    eth->type = HTONS(ETH_TYPE_ARP);

    arp->hw_type    = HTONS(1);
    arp->proto_type = HTONS(ETH_TYPE_IPV4);
    arp->hw_size    = ETH_ADDR_LEN;
    arp->proto_size = 4;
    arp->opcode     = HTONS(ARP_REQUEST);
    arp->sender_mac = iface->mac;
    arp->sender_ip  = HTONL(iface->ip);
    arp->target_ip  = HTONL(target_ip);

    if (iface->send) iface->send(iface, pkt, sizeof(pkt));
    stats.arp_requests++;
}

static void arp_handle(net_iface_t *iface, const void *data, uint32_t len)
{
    if (len < sizeof(arp_pkt_t)) return;
    const arp_pkt_t *arp = (const arp_pkt_t *)data;

    ip4_addr_t sender_ip = NTOHL(arp->sender_ip);
    arp_add(sender_ip, &arp->sender_mac);

    if (NTOHS(arp->opcode) == ARP_REQUEST) {
        ip4_addr_t target_ip = NTOHL(arp->target_ip);
        if (target_ip != iface->ip) return;

        /* ARP-Reply senden */
        uint8_t pkt[sizeof(eth_hdr_t) + sizeof(arp_pkt_t)];
        eth_hdr_t *eth  = (eth_hdr_t *)pkt;
        arp_pkt_t *resp = (arp_pkt_t *)(pkt + sizeof(eth_hdr_t));

        eth->dst  = arp->sender_mac;
        eth->src  = iface->mac;
        eth->type = HTONS(ETH_TYPE_ARP);

        resp->hw_type    = HTONS(1);
        resp->proto_type = HTONS(ETH_TYPE_IPV4);
        resp->hw_size    = ETH_ADDR_LEN;
        resp->proto_size = 4;
        resp->opcode     = HTONS(ARP_REPLY);
        resp->sender_mac = iface->mac;
        resp->sender_ip  = HTONL(iface->ip);
        resp->target_mac = arp->sender_mac;
        resp->target_ip  = arp->sender_ip;

        if (iface->send) iface->send(iface, pkt, sizeof(pkt));
        stats.arp_replies++;
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  ICMP – Feature #64
 * ══════════════════════════════════════════════════════════════════ */
static void icmp_handle(net_iface_t *iface,
                         const ip4_hdr_t *ip,
                         const void *data, uint32_t len)
{
    if (len < sizeof(icmp_hdr_t)) return;
    const icmp_hdr_t *icmp = (const icmp_hdr_t *)data;

    if (icmp->type == ICMP_ECHO_REQUEST) {
        stats.icmp_pings++;

        /* Echo-Reply zusammenbauen */
        uint8_t *reply = (uint8_t *)kmalloc(len);
        if (!reply) return;
        memcpy(reply, data, len);

        icmp_hdr_t *rep = (icmp_hdr_t *)reply;
        rep->type     = ICMP_ECHO_REPLY;
        rep->checksum = 0;
        rep->checksum = ip_checksum(reply, len);

        ip4_send(iface, NTOHL(ip->src), IP_PROTO_ICMP, reply, len);
        kfree(reply);
        stats.icmp_pongs++;
    }
}

int icmp_ping(net_iface_t *iface, ip4_addr_t target, uint16_t seq)
{
    uint8_t buf[sizeof(icmp_hdr_t) + 32];
    memset(buf, 0xAB, sizeof(buf));

    icmp_hdr_t *icmp = (icmp_hdr_t *)buf;
    icmp->type     = ICMP_ECHO_REQUEST;
    icmp->code     = 0;
    icmp->id       = HTONS(0x1234);
    icmp->seq      = HTONS(seq);
    icmp->checksum = 0;
    icmp->checksum = ip_checksum(buf, sizeof(buf));

    return ip4_send(iface, target, IP_PROTO_ICMP, buf, sizeof(buf));
}

/* ══════════════════════════════════════════════════════════════════
 *  IPv4 – Feature #63
 * ══════════════════════════════════════════════════════════════════ */
static void ipv4_handle(net_iface_t *iface, const void *data, uint32_t len)
{
    if (len < sizeof(ip4_hdr_t)) return;
    const ip4_hdr_t *ip = (const ip4_hdr_t *)data;

    uint32_t ihl    = (ip->version_ihl & 0x0F) * 4;
    uint32_t totlen = NTOHS(ip->total_len);

    if (totlen > len || ihl < 20) return;

    const uint8_t *payload = (const uint8_t *)data + ihl;
    uint32_t       plen    = totlen - ihl;

    /* Nur Pakete die an uns adressiert sind (oder Broadcast) */
    ip4_addr_t dst = NTOHL(ip->dst);
    if (iface->ip && dst != iface->ip && dst != 0xFFFFFFFF) return;

    switch (ip->protocol) {
    case IP_PROTO_ICMP:
        icmp_handle(iface, ip, payload, plen);
        break;
    case IP_PROTO_UDP:
        /* An Socket-Dispatcher übergeben */
        {
            if (plen < sizeof(udp_hdr_t)) break;
            const udp_hdr_t *udp = (const udp_hdr_t *)payload;
            uint16_t dport = NTOHS(udp->dst_port);
            uint32_t dlen  = NTOHS(udp->length) - sizeof(udp_hdr_t);
            const uint8_t *udata = payload + sizeof(udp_hdr_t);

            for (int i = 0; i < SOCK_MAX; i++) {
                if (sockets[i].used &&
                    sockets[i].protocol  == IP_PROTO_UDP &&
                    sockets[i].local_port == dport)
                {
                    /* In den Rx-Puffer kopieren */
                    for (uint32_t b = 0; b < dlen; b++) {
                        uint32_t next = (sockets[i].rx_tail + 1) % SOCK_RXBUF;
                        if (next != sockets[i].rx_head)
                            sockets[i].rx_buf[sockets[i].rx_tail] = udata[b];
                        sockets[i].rx_tail = next;
                    }
                    stats.udp_recv++;
                }
            }
        }
        break;
    default:
        break;
    }
}

int ip4_send(net_iface_t *iface, ip4_addr_t dst,
             uint8_t proto, const void *payload, uint32_t len)
{
    if (!iface || !iface->send) return -1;

    uint32_t total = sizeof(eth_hdr_t) + sizeof(ip4_hdr_t) + len;
    uint8_t *pkt   = (uint8_t *)kmalloc(total);
    if (!pkt) return -1;

    /* Ziel-MAC bestimmen */
    ip4_addr_t next_hop = dst;
    if ((dst & iface->netmask) != (iface->ip & iface->netmask))
        next_hop = iface->gateway;

    mac_addr_t *dst_mac = arp_lookup(next_hop);
    if (!dst_mac) {
        arp_send_request(iface, next_hop);
        kfree(pkt);
        return -2;   /* Caller muss wiederholen */
    }

    /* Ethernet-Header */
    eth_hdr_t *eth = (eth_hdr_t *)pkt;
    eth->dst  = *dst_mac;
    eth->src  = iface->mac;
    eth->type = HTONS(ETH_TYPE_IPV4);

    /* IP-Header */
    ip4_hdr_t *ip = (ip4_hdr_t *)(pkt + sizeof(eth_hdr_t));
    ip->version_ihl = 0x45;
    ip->tos        = 0;
    ip->total_len  = HTONS((uint16_t)(sizeof(ip4_hdr_t) + len));
    uint16_t current_id = ip_id_counter++;
    ip->id         = HTONS(current_id);
    ip->flags_frag = 0;
    ip->ttl        = 64;
    ip->protocol   = proto;
    ip->checksum   = 0;
    ip->src        = HTONL(iface->ip);
    ip->dst        = HTONL(dst);
    ip->checksum   = ip_checksum(ip, sizeof(ip4_hdr_t));

    memcpy(pkt + sizeof(eth_hdr_t) + sizeof(ip4_hdr_t), payload, len);

    int ret = iface->send(iface, pkt, total);
    kfree(pkt);

    stats.tx_packets++;
    stats.tx_bytes += total;
    return ret;
}

/* ══════════════════════════════════════════════════════════════════
 *  UDP – Feature #65
 * ══════════════════════════════════════════════════════════════════ */
int udp_send(net_iface_t *iface, ip4_addr_t dst,
             uint16_t src_port, uint16_t dst_port,
             const void *data, uint32_t len)
{
    uint32_t  total = sizeof(udp_hdr_t) + len;
    uint8_t  *buf   = (uint8_t *)kmalloc(total);
    if (!buf) return -1;

    udp_hdr_t *udp = (udp_hdr_t *)buf;
    udp->src_port = HTONS(src_port);
    udp->dst_port = HTONS(dst_port);
    udp->length   = HTONS((uint16_t)total);
    udp->checksum = 0;   /* Optional für UDP/IPv4 */

    memcpy(buf + sizeof(udp_hdr_t), data, len);
    int ret = ip4_send(iface, dst, IP_PROTO_UDP, buf, total);
    kfree(buf);
    stats.udp_sent++;
    return ret;
}

/* ══════════════════════════════════════════════════════════════════
 *  Eingehende Pakete verteilen
 * ══════════════════════════════════════════════════════════════════ */
void net_receive(net_iface_t *iface, const void *data, uint32_t len)
{
    if (len < sizeof(eth_hdr_t)) return;

    stats.rx_packets++;
    stats.rx_bytes += len;

    const eth_hdr_t *eth = (const eth_hdr_t *)data;
    const void *payload  = (const uint8_t *)data + sizeof(eth_hdr_t);
    uint32_t    plen     = len - sizeof(eth_hdr_t);

    switch (NTOHS(eth->type)) {
    case ETH_TYPE_ARP:
        arp_handle(iface, payload, plen);
        break;
    case ETH_TYPE_IPV4:
        ipv4_handle(iface, payload, plen);
        break;
    default:
        stats.rx_dropped++;
        break;
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  Socket-API – Feature #68
 * ══════════════════════════════════════════════════════════════════ */
int sock_open(uint8_t protocol)
{
    for (int i = 0; i < SOCK_MAX; i++) {
        if (!sockets[i].used) {
            memset(&sockets[i], 0, sizeof(sockets[i]));
            sockets[i].used     = true;
            sockets[i].protocol = protocol;
            return i;
        }
    }
    return -1;
}

void sock_close(int fd)
{
    if (fd >= 0 && fd < SOCK_MAX)
        sockets[fd].used = false;
}

int sock_bind(int fd, uint16_t port)
{
    if (fd < 0 || fd >= SOCK_MAX || !sockets[fd].used) return -1;
    sockets[fd].local_port = port;
    return 0;
}

int sock_sendto(int fd, ip4_addr_t dst, uint16_t port,
                const void *buf, uint32_t len)
{
    if (fd < 0 || fd >= SOCK_MAX || !sockets[fd].used) return -1;
    if (iface_count == 0) return -1;
    return udp_send(ifaces[0], dst, sockets[fd].local_port, port, buf, len);
}

int32_t sock_recvfrom(int fd, void *buf, uint32_t len,
                       ip4_addr_t *from_ip, uint16_t *from_port)
{
    if (fd < 0 || fd >= SOCK_MAX || !sockets[fd].used) return -1;
    UNUSED(from_ip); UNUSED(from_port);

    socket_t *s   = &sockets[fd];
    uint32_t  got = 0;
    uint8_t  *out = (uint8_t *)buf;

    while (got < len && s->rx_head != s->rx_tail) {
        out[got++]   = s->rx_buf[s->rx_head];
        s->rx_head   = (s->rx_head + 1) % SOCK_RXBUF;
    }
    return (int32_t)got;
}

/* ── Statistiken ─────────────────────────────────────────────────── */
const net_stats_t *net_get_stats(void) { return &stats; }

void net_dump(void)
{
    KINFO("NET Statistiken:\n");
    KINFO("  TX: %llu Pakete  %llu Bytes\n", stats.tx_packets, stats.tx_bytes);
    KINFO("  RX: %llu Pakete  %llu Bytes  %llu verworfen\n",
          stats.rx_packets, stats.rx_bytes, stats.rx_dropped);
    KINFO("  ARP: %llu Req  %llu Rep\n", stats.arp_requests, stats.arp_replies);
    KINFO("  ICMP: %llu Ping  %llu Pong\n", stats.icmp_pings, stats.icmp_pongs);
    KINFO("  UDP: %llu gesendet  %llu empfangen\n",
          stats.udp_sent, stats.udp_recv);
}
