/**
 * @file sync.c
 * @brief Semaphore, Mutex, Condition Variable, RW-Lock, Wait-Queue.
 */
#include "../include/sync.h"
#include "../include/scheduler.h"
#include "../include/cpu.h"
#include "../include/klib.h"

/* ── Semaphor ────────────────────────────────────────────────────── */
void sem_init(semaphore_t *sem, int32_t initial)
{
    sem->value       = initial;
    sem->waiter_count = 0;
    spinlock_init(&sem->lock);
    memset(sem->waiters, 0, sizeof(sem->waiters));
}

void sem_wait(semaphore_t *sem)
{
    for (;;) {
        uint32_t f = spin_lock_irqsave(&sem->lock);
        if (sem->value > 0) {
            sem->value--;
            spin_unlock_irqrestore(&sem->lock, f);
            return;
        }
        /* Task schlafen legen */
        task_t *t = sched_current();
        if (t && sem->waiter_count < SEM_MAX_WAITERS) {
            sem->waiters[sem->waiter_count++] = t->id;
        }
        spin_unlock_irqrestore(&sem->lock, f);
        task_sleep(1);   /* Kurz schlafen, dann erneut versuchen */
    }
}

bool sem_trywait(semaphore_t *sem)
{
    uint32_t f = spin_lock_irqsave(&sem->lock);
    bool ok = (sem->value > 0);
    if (ok) sem->value--;
    spin_unlock_irqrestore(&sem->lock, f);
    return ok;
}

void sem_post(semaphore_t *sem)
{
    uint32_t f = spin_lock_irqsave(&sem->lock);
    sem->value++;
    /* Einen wartenden Task aufwecken */
    if (sem->waiter_count > 0) {
        sem->waiter_count--;
        /* Der Task wird beim nächsten Scheduler-Lauf aufgeweckt */
    }
    spin_unlock_irqrestore(&sem->lock, f);
}

/* ── Mutex ───────────────────────────────────────────────────────── */
void mutex_init(mutex_t *m)
{
    m->owner_tid   = 0;
    m->depth       = 0;
    m->waiter_count = 0;
    spinlock_init(&m->lock);
    memset(m->waiters, 0, sizeof(m->waiters));
}

void mutex_lock(mutex_t *m)
{
    task_t *self = sched_current();
    uint32_t my_id = self ? self->id : 0;

    for (;;) {
        uint32_t f = spin_lock_irqsave(&m->lock);

        /* Rekursiver Lock desselben Owners */
        if (m->owner_tid == my_id) {
            m->depth++;
            spin_unlock_irqrestore(&m->lock, f);
            return;
        }
        /* Lock frei? */
        if (m->owner_tid == 0) {
            m->owner_tid = my_id;
            m->depth     = 1;
            spin_unlock_irqrestore(&m->lock, f);
            return;
        }
        /* In Warteschlange eintragen */
        if (m->waiter_count < SEM_MAX_WAITERS && my_id)
            m->waiters[m->waiter_count++] = my_id;

        spin_unlock_irqrestore(&m->lock, f);
        task_yield();
    }
}

bool mutex_trylock(mutex_t *m)
{
    task_t *self  = sched_current();
    uint32_t my_id = self ? self->id : 0;
    uint32_t f    = spin_lock_irqsave(&m->lock);
    bool ok = (m->owner_tid == 0 || m->owner_tid == my_id);
    if (ok) { m->owner_tid = my_id; m->depth++; }
    spin_unlock_irqrestore(&m->lock, f);
    return ok;
}

void mutex_unlock(mutex_t *m)
{
    uint32_t f = spin_lock_irqsave(&m->lock);
    if (m->depth > 1) {
        m->depth--;
    } else {
        m->owner_tid = 0;
        m->depth     = 0;
        if (m->waiter_count > 0)
            m->waiter_count--;
    }
    spin_unlock_irqrestore(&m->lock, f);
}

/* ── Condition Variable ──────────────────────────────────────────── */
void condvar_init(condvar_t *cv)
{
    spinlock_init(&cv->lock);
    cv->waiter_count = 0;
    memset(cv->waiters, 0, sizeof(cv->waiters));
}

void condvar_wait(condvar_t *cv, mutex_t *m)
{
    task_t *t = sched_current();
    uint32_t f = spin_lock_irqsave(&cv->lock);
    if (t && cv->waiter_count < SEM_MAX_WAITERS)
        cv->waiters[cv->waiter_count++] = t->id;
    spin_unlock_irqrestore(&cv->lock, f);
    mutex_unlock(m);
    task_sleep(5);
    mutex_lock(m);
}

void condvar_signal(condvar_t *cv)
{
    uint32_t f = spin_lock_irqsave(&cv->lock);
    if (cv->waiter_count > 0) cv->waiter_count--;
    spin_unlock_irqrestore(&cv->lock, f);
}

void condvar_broadcast(condvar_t *cv)
{
    uint32_t f = spin_lock_irqsave(&cv->lock);
    cv->waiter_count = 0;
    spin_unlock_irqrestore(&cv->lock, f);
}

/* ── RW-Lock ─────────────────────────────────────────────────────── */
void rwlock_init(rwlock_t *rw)
{
    spinlock_init(&rw->lock);
    rw->readers = 0;
    sem_init(&rw->write_sem, 1);
    sem_init(&rw->read_sem,  1);
}

void rwlock_read_lock(rwlock_t *rw)
{
    uint32_t f = spin_lock_irqsave(&rw->lock);
    rw->readers++;
    if (rw->readers == 1)
        sem_wait(&rw->write_sem);   /* Ersten Leser sperrt Schreiber  */
    spin_unlock_irqrestore(&rw->lock, f);
}

void rwlock_read_unlock(rwlock_t *rw)
{
    uint32_t f = spin_lock_irqsave(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)
        sem_post(&rw->write_sem);   /* Letzter Leser gibt Schreiber frei */
    spin_unlock_irqrestore(&rw->lock, f);
}

void rwlock_write_lock(rwlock_t *rw)  { sem_wait(&rw->write_sem); }
void rwlock_write_unlock(rwlock_t *rw){ sem_post(&rw->write_sem); }

/* ── Wait Queue ──────────────────────────────────────────────────── */
void waitq_init(wait_queue_t *wq)
{
    spinlock_init(&wq->lock);
    wq->count = 0;
    memset(wq->task_ids, 0, sizeof(wq->task_ids));
}

void waitq_wait(wait_queue_t *wq)
{
    task_t *t = sched_current();
    uint32_t f = spin_lock_irqsave(&wq->lock);
    if (t && wq->count < WAITQ_MAX)
        wq->task_ids[wq->count++] = t->id;
    spin_unlock_irqrestore(&wq->lock, f);
    task_sleep(10);
}

void waitq_wake_one(wait_queue_t *wq)
{
    uint32_t f = spin_lock_irqsave(&wq->lock);
    if (wq->count > 0) wq->count--;
    spin_unlock_irqrestore(&wq->lock, f);
}

void waitq_wake_all(wait_queue_t *wq)
{
    uint32_t f = spin_lock_irqsave(&wq->lock);
    wq->count = 0;
    spin_unlock_irqrestore(&wq->lock, f);
}
