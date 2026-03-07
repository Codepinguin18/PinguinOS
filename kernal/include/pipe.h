/**
 * @file pipe.h
 * @brief Anonymous pipe IPC API for PinguinOS.
 *
 * Feature #79 – Pipes
 *
 * A pipe is a unidirectional byte-stream channel backed by a 4 KB ring
 * buffer.  pipe_create() returns two VFS file descriptors: fds[0] for
 * reading and fds[1] for writing.  Blocking reads wait until data is
 * available; blocking writes wait if the buffer is full.
 */
#ifndef _PIPE_H
#define _PIPE_H

#include "types.h"
#include "sync.h"
#include "vfs.h"

#define PIPE_BUF_SIZE  4096   /* Ring buffer capacity in bytes */
#define PIPE_MAX       32     /* Maximum concurrent pipes      */

/* ── Pipe kernel object ──────────────────────────────────────────── */
typedef struct pipe {
    uint8_t   buf[PIPE_BUF_SIZE];
    uint32_t  read_pos;    /* Consumer index  */
    uint32_t  write_pos;   /* Producer index  */
    uint32_t  count;       /* Bytes available */
    mutex_t   lock;
    condvar_t not_empty;   /* Signalled when data is written */
    condvar_t not_full;    /* Signalled when data is consumed */
    uint32_t  ref_read;    /* Number of open read ends       */
    uint32_t  ref_write;   /* Number of open write ends      */
    bool      used;
} pipe_t;

/* ── Public API ──────────────────────────────────────────────────── */

/** Initialise the pipe subsystem (call once at boot). */
void pipe_init(void);

/**
 * @brief Create an anonymous pipe.
 * @param[out] fds  fds[0] = read end, fds[1] = write end (VFS nodes).
 * @return 0 on success, -1 on failure.
 */
int pipe_create(vfs_node_t **read_end, vfs_node_t **write_end);

/**
 * @brief Read up to @p len bytes from a pipe (blocks if empty).
 * @return Bytes read, 0 on broken pipe, -1 on error.
 */
int32_t pipe_read(pipe_t *p, uint8_t *buf, uint32_t len);

/**
 * @brief Write @p len bytes to a pipe (blocks if full).
 * @return Bytes written, -1 if write end was closed.
 */
int32_t pipe_write(pipe_t *p, const uint8_t *buf, uint32_t len);

/** Close one end of the pipe. */
void pipe_close_read(pipe_t *p);
void pipe_close_write(pipe_t *p);

#endif /* _PIPE_H */