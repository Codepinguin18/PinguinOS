/**
 * @file scheduler.h
 * @brief Priority scheduler for PinguinOS.
 *
 * v3.0 – Major optimisations
 * ────────────────────────────
 *  • 256-level priority bitmap (8×uint32_t) → O(1) highest-ready-task lookup
 *  • PIT at 1000 Hz → 1 ms tick resolution
 *  • Quantum tracking per-task (SCHED_QUANTUM_TICKS)
 *  • Per-task runtime statistics (total_ticks, context_switches, last_run_ms)
 *  • MLFQ: 4 run-queues (RT / HIGH / NORMAL / LOW) with aging
 *  • task_create_prio(), task_set_priority()
 *  • Zombie reaper: stacks auto-freed after ZOMBIE_REAP_TICKS
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "idt.h"

/* ── Configuration ───────────────────────────────────────────────── */
#define SCHED_MAX_TASKS      64
#define SCHED_STACK_SIZE     8192    /* 8 KB per kernel thread             */
#define SCHED_QUANTUM_TICKS  10      /* Ticks before forced preemption     */
#define SCHED_PIT_HZ         1000   /* 1 ms tick resolution               */
#define ZOMBIE_REAP_TICKS    200    /* Reap zombie stacks after 200 ms    */
#define AGING_THRESHOLD      500    /* Ticks before priority boost        */

/* ── Priority constants ──────────────────────────────────────────── */
#define TASK_PRIO_IDLE       0
#define TASK_PRIO_LOW        64
#define TASK_PRIO_NORMAL     128
#define TASK_PRIO_HIGH       192
#define TASK_PRIO_RT         255

/* ── MLFQ queue indices ──────────────────────────────────────────── */
#define MLFQ_RT      3    /* Real-time  (prio 192–255) */
#define MLFQ_HIGH    2    /* High       (prio 128–191) */
#define MLFQ_NORMAL  1    /* Normal     (prio  64–127) */
#define MLFQ_LOW     0    /* Low/Idle   (prio   0–63)  */

/* ── Task states ─────────────────────────────────────────────────── */
typedef enum {
    TASK_UNUSED   = 0,
    TASK_READY    = 1,
    TASK_RUNNING  = 2,
    TASK_BLOCKED  = 3,
    TASK_ZOMBIE   = 4,
    TASK_SLEEPING = 5,
} task_state_t;

/* ── Saved CPU context ───────────────────────────────────────────── */
typedef struct {
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t eip;
    uint32_t eflags;
} task_context_t;

/* ── Task control block (TCB) ────────────────────────────────────── */
typedef struct task {
    /* Identity */
    uint32_t      id;
    char          name[32];
    task_state_t  state;

    /* Context */
    task_context_t ctx;
    uint32_t      stack_top;
    uint32_t      stack_phys;

    /* Scheduling */
    uint8_t       priority;         /* Current effective priority (0–255) */
    uint8_t       base_priority;    /* Priority assigned at creation       */
    uint32_t      quantum_ticks;    /* Ticks remaining in current slice    */
    uint32_t      wait_ticks;       /* Ticks spent waiting (for aging)     */
    uint32_t      run_ticks;        /* Ticks spent running this slice      */

    /* Sleep */
    uint32_t      sleep_until_ms;

    /* Statistics */
    uint64_t      total_ticks;       /* Lifetime CPU ticks              */
    uint32_t      context_switches;  /* Number of times scheduled       */
    uint32_t      last_run_ms;       /* Uptime_ms when last ran         */
    uint32_t      created_ms;        /* Uptime_ms when created          */

    /* Exit */
    int           exit_code;
    uint32_t      zombie_ticks;      /* Ticks since entering ZOMBIE     */
} task_t;

typedef void (*task_fn_t)(void);

/* ── External context switch (isr.S) ────────────────────────────── */
extern void sched_switch_context(task_context_t *old_ctx,
                                  task_context_t *new_ctx);

/* ── Public API ──────────────────────────────────────────────────── */
void     sched_init(void);
task_t  *task_create(task_fn_t fn, const char *name);
task_t  *task_create_prio(task_fn_t fn, const char *name, uint8_t priority);
void     task_set_priority(task_t *t, uint8_t priority);
void     NORETURN task_exit(int exit_code);
void     task_yield(void);
void     task_sleep(uint32_t ms);
void     task_wake(task_t *t);
task_t  *sched_current(void);
uint32_t sched_task_count(void);
uint32_t sched_uptime_ms(void);
void     sched_dump(void);

#endif /* _SCHEDULER_H */