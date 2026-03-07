/**
 * @file dns.h
 * @brief DNS client (RFC 1035) API for PinguinOS.
 *
 * Feature #70 – DNS Client
 */
#ifndef _DNS_H
#define _DNS_H

#include "types.h"
#include "net.h"

/**
 * @brief Set the DNS resolver address (default 8.8.8.8).
 */
void dns_set_server(ip4_addr_t server);

/**
 * @brief Resolve a hostname to an IPv4 address (blocking).
 *
 * Sends a UDP DNS query to the configured server and waits up to
 * @p timeout_ms for a reply.
 *
 * @param hostname  Null-terminated hostname (e.g. "example.com").
 * @param[out] out  Resolved IP address on success.
 * @param timeout_ms  Maximum wait time in milliseconds.
 * @return true if resolution succeeded, false otherwise.
 */
bool dns_resolve(const char *hostname, ip4_addr_t *out, uint32_t timeout_ms);

#endif /* _DNS_H */