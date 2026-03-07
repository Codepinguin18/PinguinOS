/**
 * @file scheduler.h
 * @brief Round-robin preemptive scheduler for PinguinOS.
 *
 * Each task is a lightweight kernel thread.  Context switches happen on
 * the timer IRQ (IRQ0, i.e. the PIT).  Tasks can be in one of the
 * following states:
 *
 *   TASK_READY    – can be scheduled
 *   TASK_RUNNING  – currently executing
 *   TASK_BLOCKED  – waiting for an event (sleep, I/O …)
 *   TASK_ZOMBIE   – finished, waiting for cleanup
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "idt.h"

/* ── Configuration ───────────────────────────────────────────────── */
#define SCHED_MAX_TASKS    64      /* Maximum simultaneous tasks        */
#define SCHED_STACK_SIZE   8192   /* Default kernel-thread stack (8 KB) */
#define SCHED_QUANTUM_MS   10     /* Time slice in milliseconds         */

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
 * @brief Create a new kernel task.
 * @param fn    Entry-point function.
 * @param name  Human-readable task name (max 31 chars).
 * @return Pointer to the new task's TCB, or NULL on failure.
 */
task_t *task_create(task_fn_t fn, const char *name);

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
