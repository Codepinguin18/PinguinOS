/**
 * @file scheduler.h
 * @brief Priority-based preemptive scheduler for PinguinOS.
 *
 * Each task is a lightweight kernel thread.  Context switches happen on
 * the timer IRQ (IRQ0, i.e. the PIT).  Tasks can be in one of the
 * following states:
 *
 *   TASK_READY    – can be scheduled
 *   TASK_RUNNING  – currently executing
 *   TASK_BLOCKED  – waiting for an event (sleep, I/O …)
 *   TASK_ZOMBIE   – finished, waiting for cleanup
 *
 * Feature #13: Priority scheduling
 *   Higher numeric priority = runs first.
 *   Equal-priority tasks share the CPU in round-robin.
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "idt.h"

/* ── Configuration ───────────────────────────────────────────────── */
#define SCHED_MAX_TASKS    64      /* Maximum simultaneous tasks        */
#define SCHED_STACK_SIZE   8192   /* Default kernel-thread stack (8 KB) */
#define SCHED_QUANTUM_MS   10     /* Default time slice in ms          */

/* ── Priority levels ─────────────────────────────────────────────── */
#define TASK_PRIO_IDLE      0     /* Idle task only                    */
#define TASK_PRIO_LOW      64     /* Background work                   */
#define TASK_PRIO_NORMAL  128     /* Default for user tasks            */
#define TASK_PRIO_HIGH    192     /* I/O bound or interactive          */
#define TASK_PRIO_RT      255     /* Soft real-time                    */

/* ── Task states ─────────────────────────────────────────────────── */
typedef enum task_state {
    TASK_UNUSED  = 0,
    TASK_READY   = 1,
    TASK_RUNNING = 2,
    TASK_BLOCKED = 3,
    TASK_ZOMBIE  = 4,
} task_state_t;

/* ── Saved CPU context (must match isr.S push layout) ────────────── */
typedef struct task_context {
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t eip;
    uint32_t eflags;
} task_context_t;

/* ── Task control block (TCB) ────────────────────────────────────── */
typedef struct task {
    uint32_t      id;              /* Unique task ID                   */
    task_state_t  state;
    task_context_t ctx;            /* Saved register state             */
    uint32_t      stack_top;       /* Virtual address of stack top     */
    uint32_t      stack_phys;      /* Physical address of stack pages  */
    uint32_t      sleep_until_ms;  /* Wake time (when BLOCKED)         */
    char          name[32];        /* Human-readable task name         */
    int           exit_code;

    /* ── Priority scheduling (Feature #13) ──────────────────────── */
    uint8_t       priority;        /* Current priority  (0–255)        */
    uint8_t       base_priority;   /* Static/base priority             */
    uint32_t      quantum_ticks;   /* Remaining ticks in this slice    */
    uint32_t      run_ticks;       /* Total ticks ever run             */
    uint32_t      wait_ticks;      /* Ticks spent waiting (aging base) */
} task_t;

/** Entry-point signature for a kernel task. */
typedef void (*task_fn_t)(void);

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Initialise the scheduler and create the idle task.
 *
 * Must be called after pmm_init() and paging_init().
 * Hooks into IRQ0 (timer) for preemption.
 */
void sched_init(void);

/**
 * @brief Create a new kernel task with default (NORMAL) priority.
 * @param fn    Entry-point function.
 * @param name  Human-readable task name (max 31 chars).
 * @return Pointer to the new task's TCB, or NULL on failure.
 */
task_t *task_create(task_fn_t fn, const char *name);

/**
 * @brief Create a new kernel task with an explicit priority.
 * @param fn       Entry-point function.
 * @param name     Human-readable task name.
 * @param priority Priority value (use TASK_PRIO_* constants).
 */
task_t *task_create_prio(task_fn_t fn, const char *name, uint8_t priority);

/**
 * @brief Change a task's base (and current) priority.
 * @param t         Target task (NULL = current task).
 * @param priority  New priority.
 */
void task_set_priority(task_t *t, uint8_t priority);

/**
 * @brief Terminate the current task.
 * @param exit_code  Exit status code.
 *
 * This function does not return.
 */
void NORETURN task_exit(int exit_code);

/** Yield the CPU to the next ready task immediately. */
void task_yield(void);

/**
 * @brief Block the current task for at least @p ms milliseconds.
 * @param ms  Sleep duration in milliseconds.
 */
void task_sleep(uint32_t ms);

/**
 * @brief Wake a blocked task.
 * @param t  Task to wake (must be in TASK_BLOCKED state).
 */
void task_wake(task_t *t);

/** @return Pointer to the currently running task's TCB. */
task_t *sched_current(void);

/** @return The number of tasks currently in TASK_READY or TASK_RUNNING. */
uint32_t sched_task_count(void);

/** @return Milliseconds since the kernel timer was started. */
uint32_t sched_uptime_ms(void);

/** Print a table of all tasks to the serial log. */
void sched_dump(void);

#endif /* _SCHEDULER_H */