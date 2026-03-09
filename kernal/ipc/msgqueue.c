/**
 * @file msgqueue.c
 * @brief POSIX-style message queues – Feature #81.
 */

#include "../include/msgqueue.h"
#include "../include/klib.h"
#include "../include/serial.h"

static mq_queue_t queues[MQ_MAX_QUEUES];
static uint32_t   next_qid = 1;

void mq_init(void)
{
    memset(queues, 0, sizeof(queues));
    KINFO("msgqueue: initialised (%u queues × %u messages)\n",
          MQ_MAX_QUEUES, MQ_MAX_MSGS);
}

int mq_open(const char *name)
{
    /* Find existing queue by name */
    for (int i = 0; i < MQ_MAX_QUEUES; i++)
        if (queues[i].used && strncmp(queues[i].name, name, MQ_NAME_LEN-1) == 0)
            return (int)queues[i].id;

    /* Create new */
    for (int i = 0; i < MQ_MAX_QUEUES; i++) {
        mq_queue_t *q = &queues[i];
        if (q->used) continue;
        memset(q, 0, sizeof(*q));
        strncpy(q->name, name, MQ_NAME_LEN - 1);
        q->id   = next_qid++;
        q->used = true;
        mutex_init(&q->lock);
        condvar_init(&q->not_empty);
        condvar_init(&q->not_full);
        KINFO("mq: opened '%s' (id=%u)\n", name, q->id);
        return (int)q->id;
    }
    return -1;
}

void mq_close(int qid)
{
    for (int i = 0; i < MQ_MAX_QUEUES; i++)
        if (queues[i].used && (int)queues[i].id == qid)
            { queues[i].used = false; return; }
}

int mq_send(int qid, const void *data, uint32_t len, uint8_t priority)
{
    mq_queue_t *q = NULL;
    for (int i = 0; i < MQ_MAX_QUEUES; i++)
        if (queues[i].used && (int)queues[i].id == qid) { q = &queues[i]; break; }
    if (!q) return -1;
    if (len > MQ_MAX_MSG_SIZE) len = MQ_MAX_MSG_SIZE;

    mutex_lock(&q->lock);
    while (q->msg_count >= MQ_MAX_MSGS)
        condvar_wait(&q->not_full, &q->lock);

    /* Insert sorted by priority (higher priority → lower index) */
    int ins = (int)q->msg_count;
    for (int i = 0; i < (int)q->msg_count; i++) {
        if (q->msgs[i].priority < priority) { ins = i; break; }
    }
    /* Shift messages down */
    for (int i = (int)q->msg_count; i > ins; i--)
        q->msgs[i] = q->msgs[i - 1];

    mq_msg_t *m = &q->msgs[ins];
    memcpy(m->data, data, len);
    m->len      = len;
    m->priority = priority;
    m->used     = true;
    q->msg_count++;

    condvar_signal(&q->not_empty);
    mutex_unlock(&q->lock);
    return 0;
}

int mq_recv(int qid, void *buf, uint32_t maxlen)
{
    mq_queue_t *q = NULL;
    for (int i = 0; i < MQ_MAX_QUEUES; i++)
        if (queues[i].used && (int)queues[i].id == qid) { q = &queues[i]; break; }
    if (!q) return -1;

    mutex_lock(&q->lock);
    while (q->msg_count == 0)
        condvar_wait(&q->not_empty, &q->lock);

    /* Take head (highest priority = index 0) */
    mq_msg_t *m = &q->msgs[0];
    uint32_t  n  = m->len < maxlen ? m->len : maxlen;
    memcpy(buf, m->data, n);

    /* Shift remaining messages */
    for (uint32_t i = 1; i < q->msg_count; i++) q->msgs[i-1] = q->msgs[i];
    q->msg_count--;

    condvar_signal(&q->not_full);
    mutex_unlock(&q->lock);
    return (int)n;
}

int mq_tryrecv(int qid, void *buf, uint32_t maxlen)
{
    mq_queue_t *q = NULL;
    for (int i = 0; i < MQ_MAX_QUEUES; i++)
        if (queues[i].used && (int)queues[i].id == qid) { q = &queues[i]; break; }
    if (!q) return -1;

    mutex_lock(&q->lock);
    if (q->msg_count == 0) { mutex_unlock(&q->lock); return 0; }

    mq_msg_t *m = &q->msgs[0];
    uint32_t n = m->len < maxlen ? m->len : maxlen;
    memcpy(buf, m->data, n);
    for (uint32_t i = 1; i < q->msg_count; i++) q->msgs[i-1] = q->msgs[i];
    q->msg_count--;
    condvar_signal(&q->not_full);
    mutex_unlock(&q->lock);
    return (int)n;
}

void mq_dump(void)
{
    serial_puts("[MQ] Message queues:\n");
    for (int i = 0; i < MQ_MAX_QUEUES; i++) {
        mq_queue_t *q = &queues[i];
        if (!q->used) continue;
        char buf[80];
        snprintf(buf, sizeof(buf), "  id=%u '%s'  msgs=%u/%u\n",
                 q->id, q->name, q->msg_count, MQ_MAX_MSGS);
        serial_puts(buf);
    }
}