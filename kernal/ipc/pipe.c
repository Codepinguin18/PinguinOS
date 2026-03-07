/**
 * @file pipe.c
 * @brief Anonymous pipe IPC for PinguinOS.
 *
 * Feature #79 – Pipes
 */

#include "../include/pipe.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"

/* ── Global pipe table ───────────────────────────────────────────── */
static pipe_t pipe_table[PIPE_MAX];

/* ── VFS ops for the read end ────────────────────────────────────── */
static int32_t pipe_vfs_read(vfs_node_t *node, uint32_t offset,
                              uint32_t size, uint8_t *buf)
{
    (void)offset;
    return pipe_read((pipe_t *)node->fs_data, buf, size);
}

static int32_t pipe_vfs_write(vfs_node_t *node, uint32_t offset,
                               uint32_t size, const uint8_t *buf)
{
    (void)offset;
    return pipe_write((pipe_t *)node->fs_data, buf, size);
}

static fs_ops_t pipe_read_ops = {
    .name  = "pipe_r",
    .read  = pipe_vfs_read,
    .write = NULL,
};

static fs_ops_t pipe_write_ops = {
    .name  = "pipe_w",
    .read  = NULL,
    .write = pipe_vfs_write,
};

/* ── Public: pipe_init ───────────────────────────────────────────── */
void pipe_init(void)
{
    memset(pipe_table, 0, sizeof(pipe_table));
    KINFO("Pipe: subsystem initialised (%u slots)\n", PIPE_MAX);
}

/* ── Public: pipe_create ─────────────────────────────────────────── */
int pipe_create(vfs_node_t **read_end, vfs_node_t **write_end)
{
    /* Find a free slot */
    pipe_t *p = NULL;
    for (int i = 0; i < PIPE_MAX; i++) {
        if (!pipe_table[i].used) { p = &pipe_table[i]; break; }
    }
    if (!p) return -1;

    memset(p, 0, sizeof(*p));
    mutex_init(&p->lock);
    condvar_init(&p->not_empty);
    condvar_init(&p->not_full);
    p->ref_read  = 1;
    p->ref_write = 1;
    p->used      = true;

    /* Allocate VFS nodes */
    vfs_node_t *rnode = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    vfs_node_t *wnode = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!rnode || !wnode) {
        kfree(rnode); kfree(wnode);
        p->used = false;
        return -1;
    }

    memcpy(rnode->name, "pipe:r", 7);
    rnode->flags   = VFS_PIPE;
    rnode->ops     = &pipe_read_ops;
    rnode->fs_data = p;

    memcpy(wnode->name, "pipe:w", 7);
    wnode->flags   = VFS_PIPE;
    wnode->ops     = &pipe_write_ops;
    wnode->fs_data = p;

    *read_end  = rnode;
    *write_end = wnode;
    return 0;
}

/* ── Public: pipe_read ───────────────────────────────────────────── */
int32_t pipe_read(pipe_t *p, uint8_t *buf, uint32_t len)
{
    mutex_lock(&p->lock);

    /* Wait until data is available or write end is closed */
    while (p->count == 0) {
        if (p->ref_write == 0) {
            mutex_unlock(&p->lock);
            return 0;   /* Broken pipe (EOF) */
        }
        condvar_wait(&p->not_empty, &p->lock);
    }

    uint32_t n = (len < p->count) ? len : p->count;
    for (uint32_t i = 0; i < n; i++) {
        buf[i] = p->buf[p->read_pos % PIPE_BUF_SIZE];
        p->read_pos++;
    }
    p->count -= n;

    condvar_broadcast(&p->not_full);
    mutex_unlock(&p->lock);
    return (int32_t)n;
}

/* ── Public: pipe_write ──────────────────────────────────────────── */
int32_t pipe_write(pipe_t *p, const uint8_t *buf, uint32_t len)
{
    mutex_lock(&p->lock);

    if (p->ref_read == 0) {
        mutex_unlock(&p->lock);
        return -1;   /* No readers – SIGPIPE equivalent */
    }

    uint32_t written = 0;
    while (written < len) {
        /* Wait if buffer is full */
        while (p->count == PIPE_BUF_SIZE) {
            if (p->ref_read == 0) { mutex_unlock(&p->lock); return -1; }
            condvar_wait(&p->not_full, &p->lock);
        }

        uint32_t space = PIPE_BUF_SIZE - p->count;
        uint32_t chunk = (len - written < space) ? len - written : space;

        for (uint32_t i = 0; i < chunk; i++) {
            p->buf[p->write_pos % PIPE_BUF_SIZE] = buf[written + i];
            p->write_pos++;
        }
        p->count  += chunk;
        written   += chunk;
        condvar_broadcast(&p->not_empty);
    }

    mutex_unlock(&p->lock);
    return (int32_t)written;
}

/* ── Public: pipe_close_* ────────────────────────────────────────── */
void pipe_close_read(pipe_t *p)
{
    mutex_lock(&p->lock);
    if (p->ref_read) p->ref_read--;
    condvar_broadcast(&p->not_full);
    bool dead = (p->ref_read == 0 && p->ref_write == 0);
    mutex_unlock(&p->lock);
    if (dead) p->used = false;
}

void pipe_close_write(pipe_t *p)
{
    mutex_lock(&p->lock);
    if (p->ref_write) p->ref_write--;
    condvar_broadcast(&p->not_empty);
    bool dead = (p->ref_read == 0 && p->ref_write == 0);
    mutex_unlock(&p->lock);
    if (dead) p->used = false;
}