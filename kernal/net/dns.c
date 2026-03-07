/**
 * @file dns.c
 * @brief DNS-Client (RFC 1035) für PinguinOS.
 *
 * Feature #70 – DNS-Client
 *
 * Implementiert einen minimalen A-Record-Resolver:
 *   1. Erstellt ein DNS-Abfragepaket (einzelne A-Anfrage).
 *   2. Sendet dieses über UDP an den konfigurierten DNS-Server auf Port 53.
 *   3. Parst den ersten A-Record in der Antwort.
 *
 * Dies ist ein synchroner Resolver für einzelne Fragen ohne Caching.
 */

#include "../include/dns.h"
#include "../include/net.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"
#include "../include/entropy.h"

/* ── DNS-Protokoll-Konstanten ────────────────────────────────────── */
#define DNS_PORT        53
#define DNS_CLIENT_PORT 5353   /* Ephemeraler Quellport */
#define DNS_MAX_PKT     512
#define DNS_TYPE_A      1
#define DNS_CLASS_IN    1

/* DNS-Header */
typedef struct __attribute__((packed)) {
    uint16_t id;
    uint16_t flags;      /* QR|Opcode|AA|TC|RD|RA|Z|RCODE */
    uint16_t qdcount;    /* Fragen      */
    uint16_t ancount;    /* Antworten   */
    uint16_t nscount;    /* Autorität   */
    uint16_t arcount;    /* Zusatzeintr. */
} dns_hdr_t;

/* ── Modul-Zustand ───────────────────────────────────────────────── */
static ip4_addr_t dns_server = IP4(8, 8, 8, 8);   /* Google DNS Default */
static uint16_t   dns_txid   = 0x1234;

/* ── Öffentlich: dns_set_server ──────────────────────────────────── */
void dns_set_server(ip4_addr_t server)
{
    dns_server = server;
    KINFO("DNS: Server gesetzt auf %u.%u.%u.%u\n",
          (server>>24)&0xFF,(server>>16)&0xFF,
          (server>>8)&0xFF, server&0xFF);
}

/* ── DNS-Name im Label-Format kodieren ────────────────────────────── */
static uint32_t dns_encode_name(const char *hostname, uint8_t *out)
{
    uint8_t *start = out;
    const char *p  = hostname;

    while (*p) {
        const char *dot = p;
        while (*dot && *dot != '.') dot++;
        uint8_t label_len = (uint8_t)(dot - p);
        *out++ = label_len;
        memcpy(out, p, label_len);
        out += label_len;
        if (*dot == '.') dot++;
        p = dot;
    }
    *out++ = 0;   /* Root label */
    return (uint32_t)(out - start);
}

/* ── DNS-Name dekodieren (unterstützt Pointer-Kompression) ───────── */
static uint32_t dns_decode_name(const uint8_t *pkt, uint32_t pkt_len,
                                 uint32_t offset, char *out, uint32_t out_max)
{
    uint32_t consumed = 0;
    uint32_t pos      = offset;
    bool     jumped   = false;
    uint32_t out_pos  = 0;

    while (pos < pkt_len) {
        uint8_t len = pkt[pos];

        if ((len & 0xC0) == 0xC0) {
            /* Pointer */
            if (pos + 1 >= pkt_len) break;
            uint16_t ptr = (uint16_t)(((len & 0x3F) << 8) | pkt[pos + 1]);
            if (!jumped) consumed = pos - offset + 2;
            jumped = true;
            pos    = ptr;
            continue;
        }

        if (len == 0) {
            if (!jumped) consumed = pos - offset + 1;
            break;
        }

        pos++;
        for (uint8_t i = 0; i < len && out_pos + 1 < out_max; i++)
            out[out_pos++] = (char)pkt[pos++];
        if (out_pos < out_max) out[out_pos++] = '.';
    }

    if (out_pos > 0 && out[out_pos - 1] == '.') out_pos--;
    out[out_pos] = '\0';
    return consumed;
}

/* ── Öffentlich: dns_resolve ─────────────────────────────────────── */
bool dns_resolve(const char *hostname, ip4_addr_t *out, uint32_t timeout_ms)
{
    /* UDP-Socket öffnen */
    int sock = sock_open(IP_PROTO_UDP);
    if (sock < 0) return false;
    sock_bind(sock, (uint16_t)(DNS_CLIENT_PORT + (dns_txid & 0xFF)));

    /* Abfragepaket bauen */
    uint8_t  pkt[DNS_MAX_PKT];
    uint32_t offset = 0;

    dns_hdr_t *hdr = (dns_hdr_t *)pkt;
    uint16_t id  = dns_txid++;   /* Fix: Wsequence-point */
    hdr->id      = htons(id);
    hdr->flags   = htons(0x0100);   /* RD=1 (Rekursion gewünscht) */
    hdr->qdcount = htons(1);
    hdr->ancount = 0;
    hdr->nscount = 0;
    hdr->arcount = 0;
    offset += sizeof(dns_hdr_t);

    /* Fragen-Bereich */
    offset += dns_encode_name(hostname, pkt + offset);

    pkt[offset++] = 0; pkt[offset++] = DNS_TYPE_A;    /* QTYPE  = A  */
    pkt[offset++] = 0; pkt[offset++] = DNS_CLASS_IN;  /* QCLASS = IN */

    /* Abfrage senden */
    if (sock_sendto(sock, dns_server, DNS_PORT, pkt, offset) < 0) {
        sock_close(sock);
        return false;
    }

    KINFO("DNS: Anfrage fuer '%s' (id=0x%04x)\n", hostname, ntohs(hdr->id));

    /* Auf Antwort warten */
    uint8_t  rxbuf[DNS_MAX_PKT];
    ip4_addr_t from_ip;
    uint16_t   from_port;
    uint32_t   deadline = (uint32_t)sched_uptime_ms() + timeout_ms;

    while ((uint32_t)sched_uptime_ms() < deadline) {
        int32_t n = sock_recvfrom(sock, rxbuf, DNS_MAX_PKT,
                                  &from_ip, &from_port);
        if (n < (int32_t)sizeof(dns_hdr_t)) { task_sleep(10); continue; }

        dns_hdr_t *rh = (dns_hdr_t *)rxbuf;
        if (rh->id != hdr->id) continue;

        /* QR-Bit muss 1 sein (Antwort), kein Fehler */
        uint16_t rflags = ntohs(rh->flags);
        if (!(rflags & 0x8000)) continue;
        if ((rflags & 0x000F) != 0) {
            KERR("DNS: Server lieferte RCODE=%u\n", rflags & 0xF);
            break;
        }

        uint16_t answers = ntohs(rh->ancount);
        if (answers == 0) break;

        /* Fragen-Bereich überspringen */
        uint32_t pos = sizeof(dns_hdr_t);
        /* Fragen-Name überspringen */
        char tmp[256];
        uint32_t skip = dns_decode_name(rxbuf, (uint32_t)n, pos, tmp, sizeof(tmp));
        pos += skip + 4;   /* +4 für QTYPE + QCLASS */

        /* Antwort-Records parsen */
        for (uint16_t i = 0; i < answers; i++) {
            if (pos + 10 > (uint32_t)n) break;

            skip = dns_decode_name(rxbuf, (uint32_t)n, pos, tmp, sizeof(tmp));
            pos += skip;

            uint16_t rtype  = (uint16_t)((rxbuf[pos] << 8) | rxbuf[pos+1]); pos += 2;
            pos += 2;   /* Klasse */
            pos += 4;   /* TTL    */
            uint16_t rdlen  = (uint16_t)((rxbuf[pos] << 8) | rxbuf[pos+1]); pos += 2;

            if (rtype == DNS_TYPE_A && rdlen == 4) {
                ip4_addr_t ip = ((uint32_t)rxbuf[pos]   << 24) |
                                ((uint32_t)rxbuf[pos+1] << 16) |
                                ((uint32_t)rxbuf[pos+2] <<  8) |
                                 (uint32_t)rxbuf[pos+3];
                *out = ip;
                sock_close(sock);
                KINFO("DNS: '%s' -> %u.%u.%u.%u\n", hostname,
                      (ip>>24)&0xFF,(ip>>16)&0xFF,(ip>>8)&0xFF,ip&0xFF);
                return true;
            }
            pos += rdlen;
        }
        break;
    }

    sock_close(sock);
    KERR("DNS: Aufloesung fehlgeschlagen fuer '%s'\n", hostname);
    return false;
}
