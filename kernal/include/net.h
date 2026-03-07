/**
 * @file net.h
 * @brief Netzwerk-Stack für PinguinOS – Ethernet, ARP, IPv4, ICMP, UDP (Features #61–75).
 *
 * Schichtenarchitektur:
 *   Anwendung (Socket-API)
 *       ↕  UDP / TCP
 *       ↕  IPv4
 *       ↕  ARP
 *       ↕  Ethernet
 *       ↕  NIC-Treiber (E1000 / RTL8139)
 */
#ifndef _NET_H
#define _NET_H

#include "types.h"

/* ── Maximale Paketgröße ─────────────────────────────────────────── */
#define ETH_MTU        1500
#define ETH_HDR_LEN    14
#define ETH_ADDR_LEN   6

/* ── Ethernet-Frame-Typen ────────────────────────────────────────── */
#define ETH_TYPE_IPV4  0x0800
#define ETH_TYPE_ARP   0x0806
#define ETH_TYPE_IPV6  0x86DD

/* ── IP-Protokollnummern ─────────────────────────────────────────── */
#define IP_PROTO_ICMP  1
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP   17

/* ── ICMP-Typen ──────────────────────────────────────────────────── */
#define ICMP_ECHO_REPLY    0
#define ICMP_ECHO_REQUEST  8

/* ── ARP-Opcodes ─────────────────────────────────────────────────── */
#define ARP_REQUEST  1
#define ARP_REPLY    2

/* ── Hilfsmakros: Byte-Order-Konvertierung ───────────────────────── */
#define HTONS(x) ((uint16_t)(((x) >> 8) | ((x) << 8)))
#define NTOHS(x) HTONS(x)
#define HTONL(x) ((uint32_t)(             \
    (((x) & 0xFF000000) >> 24) |          \
    (((x) & 0x00FF0000) >>  8) |          \
    (((x) & 0x0000FF00) <<  8) |          \
    (((x) & 0x000000FF) << 24)))
#define NTOHL(x) HTONL(x)

/* ── MAC-Adresse ─────────────────────────────────────────────────── */
typedef struct { uint8_t b[ETH_ADDR_LEN]; } mac_addr_t;

/* ── IPv4-Adresse ────────────────────────────────────────────────── */
typedef uint32_t ip4_addr_t;

#define IP4(a,b,c,d) ((ip4_addr_t)( \
    ((uint32_t)(a) << 24) | ((uint32_t)(b) << 16) | \
    ((uint32_t)(c) <<  8) | ((uint32_t)(d))))

/* ── Ethernet-Header ─────────────────────────────────────────────── */
typedef struct {
    mac_addr_t dst;
    mac_addr_t src;
    uint16_t   type;      /* Big-Endian */
} PACKED eth_hdr_t;

/* ── ARP-Paket ───────────────────────────────────────────────────── */
typedef struct {
    uint16_t   hw_type;      /* 1 = Ethernet        */
    uint16_t   proto_type;   /* 0x0800 = IPv4        */
    uint8_t    hw_size;      /* 6                    */
    uint8_t    proto_size;   /* 4                    */
    uint16_t   opcode;
    mac_addr_t sender_mac;
    ip4_addr_t sender_ip;    /* Big-Endian           */
    mac_addr_t target_mac;
    ip4_addr_t target_ip;
} PACKED arp_pkt_t;

/* ── IPv4-Header ─────────────────────────────────────────────────── */
typedef struct {
    uint8_t    version_ihl;   /* Version=4, IHL=5 (20 Bytes)          */
    uint8_t    tos;
    uint16_t   total_len;
    uint16_t   id;
    uint16_t   flags_frag;
    uint8_t    ttl;
    uint8_t    protocol;
    uint16_t   checksum;
    ip4_addr_t src;
    ip4_addr_t dst;
} PACKED ip4_hdr_t;

/* ── ICMP-Header ─────────────────────────────────────────────────── */
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
} PACKED icmp_hdr_t;

/* ── UDP-Header ──────────────────────────────────────────────────── */
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} PACKED udp_hdr_t;

/* ── Netzwerk-Interface ──────────────────────────────────────────── */
typedef struct net_iface {
    char       name[16];
    mac_addr_t mac;
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gateway;
    bool       up;

    /* Sende-Funktion des NIC-Treibers */
    int (*send)(struct net_iface *iface, const void *data, uint32_t len);
} net_iface_t;

/* ── ARP-Cache ───────────────────────────────────────────────────── */
#define ARP_CACHE_SIZE  16

typedef struct {
    ip4_addr_t ip;
    mac_addr_t mac;
    uint32_t   ttl;      /* Ticks bis zum Ablauf */
    bool       valid;
} arp_entry_t;

/* ── Socket (vereinfacht UDP/ICMP) ──────────────────────────────── */
#define SOCK_MAX     16
#define SOCK_RXBUF   2048

typedef struct {
    bool       used;
    uint8_t    protocol;    /* IP_PROTO_UDP, IP_PROTO_ICMP */
    uint16_t   local_port;
    ip4_addr_t remote_ip;
    uint16_t   remote_port;
    uint8_t    rx_buf[SOCK_RXBUF];
    uint32_t   rx_head, rx_tail;
} socket_t;

/* ── Netzwerk-Statistiken ────────────────────────────────────────── */
typedef struct {
    uint64_t tx_packets, tx_bytes;
    uint64_t rx_packets, rx_bytes;
    uint64_t rx_dropped;
    uint64_t arp_requests, arp_replies;
    uint64_t icmp_pings, icmp_pongs;
    uint64_t udp_sent, udp_recv;
} net_stats_t;

/* ══════════════════════════════════════════════════════════════════
 *  Öffentliche API
 * ══════════════════════════════════════════════════════════════════ */

/* Initialisierung */
void net_init(void);
int  net_register_iface(net_iface_t *iface);

/* Empfang (wird vom NIC-Treiber aufgerufen) */
void net_receive(net_iface_t *iface, const void *data, uint32_t len);

/* ARP */
void        arp_send_request(net_iface_t *iface, ip4_addr_t target_ip);
mac_addr_t *arp_lookup(ip4_addr_t ip);
void        arp_add(ip4_addr_t ip, const mac_addr_t *mac);

/* IPv4 */
int ip4_send(net_iface_t *iface, ip4_addr_t dst,
             uint8_t proto, const void *payload, uint32_t len);

/* ICMP */
int icmp_ping(net_iface_t *iface, ip4_addr_t target, uint16_t seq);

/* UDP */
int udp_send(net_iface_t *iface, ip4_addr_t dst,
             uint16_t src_port, uint16_t dst_port,
             const void *data, uint32_t len);

/* Socket-API */
int     sock_open(uint8_t protocol);
void    sock_close(int fd);
int     sock_bind(int fd, uint16_t port);
int     sock_sendto(int fd, ip4_addr_t dst, uint16_t port,
                    const void *buf, uint32_t len);
int32_t sock_recvfrom(int fd, void *buf, uint32_t len,
                      ip4_addr_t *from_ip, uint16_t *from_port);

/* Statistiken */
const net_stats_t *net_get_stats(void);
void net_dump(void);

#endif /* _NET_H */
