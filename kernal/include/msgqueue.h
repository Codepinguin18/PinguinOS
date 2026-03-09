/**
 * @file msgqueue.h + msgqueue.c
 * @brief POSIX-style message queues for PinguinOS – Feature #81.
 *
 * Named message queues.  Senders and receivers are decoupled: a sender
 * calls mq_send(qid, data, len, priority) and a receiver calls
 * mq_recv(qid, buf, maxlen) which blocks until a message is available.
 * Messages within a queue are ordered by priority (highest first), then
 * FIFO within the same priority.
 */
#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H

#include "types.h"
#include "sync.h"

#define MQ_MAX_QUEUES    16
#define MQ_MAX_MSGS      32
#define MQ_MAX_MSG_SIZE  512
#define MQ_NAME_LEN      32

typedef struct mq_msg {
    uint8_t  data[MQ_MAX_MSG_SIZE];
    uint32_t len;
    uint8_t  priority;     /* 0 = lowest, 255 = highest */
    bool     used;
} mq_msg_t;

typedef struct mq_queue {
    char      name[MQ_NAME_LEN];
    mq_msg_t  msgs[MQ_MAX_MSGS];
    uint32_t  msg_count;
    mutex_t   lock;
    condvar_t not_empty;
    condvar_t not_full;
    bool      used;
    uint32_t  id;
} mq_queue_t;

void mq_init(void);
int  mq_open(const char *name);       /* Create or open; returns id */
void mq_close(int qid);
int  mq_send(int qid, const void *data, uint32_t len, uint8_t priority);
int  mq_recv(int qid, void *buf, uint32_t maxlen);   /* Blocking */
int  mq_tryrecv(int qid, void *buf, uint32_t maxlen); /* Non-blocking */
void mq_dump(void);

#endif /* _MSGQUEUE_H */