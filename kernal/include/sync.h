/**
 * @file sync.h
 * @brief Spinlock, Semaphor, Mutex, Condition Variable, RW-Lock (Features #81–85)
 */
#ifndef _SYNC_H
#define _SYNC_H

#include "types.h"
#include "cpu.h"
/* ══════════════════════════════════════════════════════════════════
 *  SPINLOCK  –  Feature #81
 *  Busy-Wait-Lock für kurzfristige kritische Abschnitte.
 *  Niemals in Schleifen halten, die schlafen!
 * ══════════════════════════════════════════════════════════════════ */
typedef struct {
    volatile uint32_t locked;   /* 0 = frei, 1 = gesperrt */
} spinlock_t;

#define SPINLOCK_INIT  { .locked = 0 }

static INLINE void spinlock_init(spinlock_t *lock) { lock->locked = 0; }

static INLINE void spin_lock(spinlock_t *lock)
{
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        while (lock->locked) pause();
    }
}

static INLINE void spin_unlock(spinlock_t *lock)
{
    __sync_lock_release(&lock->locked);
}

static INLINE bool spin_trylock(spinlock_t *lock)
{
    return __sync_lock_test_and_set(&lock->locked, 1) == 0;
}

/* IRQ-sicheres Locking */
static INLINE uint32_t spin_lock_irqsave(spinlock_t *lock)
{
    uint32_t flags = irq_save();
    spin_lock(lock);
    return flags;
}

static INLINE void spin_unlock_irqrestore(spinlock_t *lock, uint32_t flags)
{
    spin_unlock(lock);
    irq_restore(flags);
}

/* ══════════════════════════════════════════════════════════════════
 *  SEMAPHOR  –  Feature #82
 *  Zählende Synchronisation mit Warteschlange.
 * ══════════════════════════════════════════════════════════════════ */
#define SEM_MAX_WAITERS  16

typedef struct {
    volatile int32_t value;
    spinlock_t       lock;
    uint32_t         waiters[SEM_MAX_WAITERS];  /* Task-IDs             */
    uint32_t         waiter_count;
} semaphore_t;

void sem_init(semaphore_t *sem, int32_t initial);
void sem_wait(semaphore_t *sem);          /* P() / down()  */
void sem_post(semaphore_t *sem);          /* V() / up()    */
bool sem_trywait(semaphore_t *sem);       /* Nicht-blockierend */

/* ══════════════════════════════════════════════════════════════════
 *  MUTEX  –  Feature #83
 *  Binäre Semaphore mit Owner-Tracking und Prioritätsvererbung.
 * ══════════════════════════════════════════════════════════════════ */
typedef struct {
    volatile uint32_t owner_tid;   /* Task-ID des aktuellen Besitzers  */
    uint32_t          depth;       /* Für rekursive Mutexes            */
    spinlock_t        lock;
    uint32_t          waiters[SEM_MAX_WAITERS];
    uint32_t          waiter_count;
} mutex_t;

#define MUTEX_INIT  { .owner_tid = 0, .depth = 0 }

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);
bool mutex_trylock(mutex_t *m);

/* ══════════════════════════════════════════════════════════════════
 *  CONDITION VARIABLE  –  Feature #84
 * ══════════════════════════════════════════════════════════════════ */
typedef struct {
    spinlock_t lock;
    uint32_t   waiters[SEM_MAX_WAITERS];
    uint32_t   waiter_count;
} condvar_t;

void condvar_init(condvar_t *cv);
void condvar_wait(condvar_t *cv, mutex_t *m);
void condvar_signal(condvar_t *cv);
void condvar_broadcast(condvar_t *cv);

/* ══════════════════════════════════════════════════════════════════
 *  READ-WRITE LOCK  –  Feature #85
 *  Viele gleichzeitige Leser, exklusiver Schreibzugriff.
 * ══════════════════════════════════════════════════════════════════ */
typedef struct {
    spinlock_t       lock;
    volatile int32_t readers;      /* > 0 = Leser aktiv, -1 = Schreiber */
    semaphore_t      write_sem;
    semaphore_t      read_sem;
} rwlock_t;

void rwlock_init(rwlock_t *rw);
void rwlock_read_lock(rwlock_t *rw);
void rwlock_read_unlock(rwlock_t *rw);
void rwlock_write_lock(rwlock_t *rw);
void rwlock_write_unlock(rwlock_t *rw);

/* ══════════════════════════════════════════════════════════════════
 *  WAIT QUEUE  –  Feature #15
 *  Task effizient schlafen legen bis ein Ereignis eintritt.
 * ══════════════════════════════════════════════════════════════════ */
#define WAITQ_MAX  16

typedef struct {
    spinlock_t lock;
    uint32_t   task_ids[WAITQ_MAX];
    uint32_t   count;
} wait_queue_t;

void waitq_init(wait_queue_t *wq);
void waitq_wait(wait_queue_t *wq);      /* Schlafen legen       */
void waitq_wake_one(wait_queue_t *wq);  /* Einen aufwecken      */
void waitq_wake_all(wait_queue_t *wq);  /* Alle aufwecken       */

#endif /* _SYNC_H */
