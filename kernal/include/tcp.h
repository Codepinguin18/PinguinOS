/**
 * @file tcp.h
 * @brief TCP (RFC 793) stack API for PinguinOS.
 *
 * Feature #66 – TCP Stack
 * Feature #67 – TCP Window / Flow Control
 *
 * Implements the full TCP state machine, connection management,
 * reliable data transfer, and a blocking socket-style API.
 */
#ifndef _TCP_H
#define _TCP_H

#include "types.h"
#include "net.h"

/* ── TCP header ──────────────────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  data_offset; /* High nibble = header length / 4 */
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} tcp_hdr_t;

/* TCP flag bits */
#define TCP_FIN  0x01
#define TCP_SYN  0x02
#define TCP_RST  0x04
#define TCP_PSH  0x08
#define TCP_ACK  0x10
#define TCP_URG  0x20

/* ── TCP state machine ───────────────────────────────────────────── */
typedef enum {
    TCP_CLOSED, TCP_LISTEN, TCP_SYN_SENT, TCP_SYN_RECEIVED,
    TCP_ESTABLISHED, TCP_FIN_WAIT_1, TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT, TCP_CLOSING, TCP_LAST_ACK, TCP_TIME_WAIT
} tcp_state_t;

/* ── Per-connection buffers ──────────────────────────────────────── */
#define TCP_RXBUF_SIZE  4096
#define TCP_TXBUF_SIZE  4096
#define TCP_MAX_CONNS   16
#define TCP_DEFAULT_WIN 8192  /* Initial receive window */
#define TCP_MSS         1460  /* Max Segment Size (ETH 1500 - IP 20 - TCP 20) */

/* ── Connection descriptor ───────────────────────────────────────── */
typedef struct tcp_conn {
    bool        used;
    tcp_state_t state;

    /* Addressing */
    ip4_addr_t  local_ip;
    uint16_t    local_port;
    ip4_addr_t  remote_ip;
    uint16_t    remote_port;

    /* Sequence numbers */
    uint32_t    snd_una;   /* Oldest unacknowledged sequence number */
    uint32_t    snd_nxt;   /* Next sequence number to send          */
    uint32_t    snd_wnd;   /* Remote receive window                 */
    uint32_t    rcv_nxt;   /* Next expected receive sequence number */
    uint32_t    rcv_wnd;   /* Our advertised receive window         */
    uint32_t    iss;       /* Initial send sequence number          */

    /* RX ring buffer */
    uint8_t     rxbuf[TCP_RXBUF_SIZE];
    uint32_t    rx_head, rx_tail;

    /* TX ring buffer */
    uint8_t     txbuf[TCP_TXBUF_SIZE];
    uint32_t    tx_head, tx_tail;

    /* Retransmission */
    uint32_t    retx_timeout_ms;
    uint32_t    retx_last_ms;
    uint32_t    retx_count;

    /* Interface (set on connect) */
    net_iface_t *iface;
} tcp_conn_t;

/* ── Public API ──────────────────────────────────────────────────── */

/** Initialise TCP subsystem; call after net_init(). */
void tcp_init(void);

/**
 * @brief Open a TCP socket (returns a connection index, or -1).
 */
int tcp_socket(void);

/**
 * @brief Connect to a remote host (blocking, timeout in ms).
 * @return 0 on success, -1 on failure/timeout.
 */
int tcp_connect(int sock, ip4_addr_t remote_ip, uint16_t remote_port,
                uint32_t timeout_ms);

/**
 * @brief Send data over an established connection.
 * @return Bytes actually queued, or -1 on error.
 */
int tcp_send(int sock, const void *data, uint32_t len);

/**
 * @brief Receive data (blocking up to timeout_ms).
 * @return Bytes received, 0 on connection close, -1 on error/timeout.
 */
int tcp_recv(int sock, void *buf, uint32_t len, uint32_t timeout_ms);

/**
 * @brief Close a TCP connection gracefully (FIN handshake).
 */
void tcp_close(int sock);

/**
 * @brief Process an incoming TCP segment; called by the IP layer.
 */
void tcp_receive(net_iface_t *iface, ip4_addr_t src_ip,
                 const tcp_hdr_t *hdr, uint32_t len);

/** Return current connection state (for debugging). */
tcp_state_t tcp_state(int sock);

/** Dump all active connections to serial. */
void tcp_dump(void);

#endif /* _TCP_H */