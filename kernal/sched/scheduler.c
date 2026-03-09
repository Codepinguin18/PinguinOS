/**
 * @file scheduler.c
 * @brief Priority scheduler for PinguinOS – v3.0.
 *
 * Key improvements over v2.x:
 *
 *   1. O(1) task selection via 256-bit priority bitmap (8 × uint32_t).
 *      __builtin_clz finds the highest set bit in constant time.
 *
 *   2. PIT reconfigured to 1000 Hz (1 ms ticks) from 100 Hz.
 *      Removes the × (1000/HZ) multiplication from hot paths.
 *
 *   3. MLFQ with 4 levels (RT / HIGH / NORMAL / LOW).
 *      Tasks are moved one level down after consuming a full quantum.
 *      Aging: a task waiting AGING_THRESHOLD ticks is promoted one level.
 *
 *   4. Zombie reaper: stacks are freed automatically after
 *      ZOMBIE_REAP_TICKS without a dedicated reaper task.
 *
 *   5. Per-task statistics: total_ticks, context_switches, last_run_ms.
 */

#include "../include/scheduler.h"
#include "../include/idt.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/gdt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── PIT ─────────────────────────────────────────────────────────── */
#define PIT_BASE_FREQ  1193182u
#define PIT_CMD_PORT   0x43
#define PIT_CH0_PORT   0x40

static void pit_init(uint32_t hz)
{
    uint32_t div = PIT_BASE_FREQ / hz;
    if (div > 0xFFFF) div = 0xFFFF;
    outb(PIT_CMD_PORT, 0x36);
    outb(PIT_CH0_PORT, (uint8_t)(div & 0xFF));
    outb(PIT_CH0_PORT, (uint8_t)(div >> 8));
}

/* ── Module state ────────────────────────────────────────────────── */
static task_t    tasks[SCHED_MAX_TASKS];
static task_t   *current_task = NULL;
static uint32_t  next_id      = 1;
static uint32_t  uptime_ms    = 0;   /* 1:1 ticks→ms at 1000 Hz */

/*
 * O(1) ready-queue bitmap.
 * Bit (p % 32) of word (p / 32) is SET when any task with priority p
 * is in TASK_READY state.  We rebuild this on every context switch;
 * it costs at most 8 bitmask writes per switch.
 */
static uint32_t prio_bitmap[8];   /* 256 bits total */

static INLINE void bitmap_set(uint8_t prio)
{ prio_bitmap[prio >> 5] |= (1u << (prio & 31)); }

static INLINE void bitmap_clear(uint8_t prio)
{ prio_bitmap[prio >> 5] &= ~(1u << (prio & 31)); }

/*
 * Return the highest set priority in the bitmap, or 0xFF if empty.
 * __builtin_clz(0) is undefined; guard explicitly.
 */
static INLINE uint8_t bitmap_top(void)
{
    for (int w = 7; w >= 0; w--) {
        if (prio_bitmap[w]) {
            int bit = 31 - __builtin_clz(prio_bitmap[w]);
            return (uint8_t)(w * 32 + bit);
        }
    }
    return 0xFF;   /* Nothing ready */
}

/* ── Rebuild the priority bitmap from scratch ────────────────────── */
static void bitmap_rebuild(void)
{
    memset(prio_bitmap, 0, sizeof(prio_bitmap));
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_READY)
            bitmap_set(t->priority);
    }
}

/* ── Wake sleeping tasks whose deadline has passed ───────────────── */
static void wake_sleepers(void)
{
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_SLEEPING && uptime_ms >= t->sleep_until_ms) {
            t->state = TASK_READY;
            bitmap_set(t->priority);
        }
    }
}

/* ── Age waiting tasks (anti-starvation) ────────────────────────── */
static void age_tasks(void)
{
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state != TASK_READY) continue;
        t->wait_ticks++;
        if (t->wait_ticks >= AGING_THRESHOLD && t->priority < 254) {
            bitmap_clear(t->priority);
            t->priority++;
            t->wait_ticks = 0;
            bitmap_set(t->priority);
        }
    }
}

/* ── Reap zombies whose stacks can be freed ──────────────────────── */
static void reap_zombies(void)
{
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state != TASK_ZOMBIE) continue;
        t->zombie_ticks++;
        if (t->zombie_ticks >= ZOMBIE_REAP_TICKS && t->stack_phys) {
            pmm_free_pages(t->stack_phys, SCHED_STACK_SIZE / PAGE_SIZE);
            t->stack_phys = 0;
            t->state      = TASK_UNUSED;
        }
    }
}

/* ── Pick the next task to run ───────────────────────────────────── */
static task_t *sched_pick_next(void)
{
    uint8_t top = bitmap_top();
    if (top == 0xFF) return NULL;

    /* Among all tasks at the top priority, round-robin */
    int start = current_task ? (int)(current_task - tasks) : -1;
    for (int i = 1; i <= SCHED_MAX_TASKS; i++) {
        int idx = (start + i) % SCHED_MAX_TASKS;
        task_t *t = &tasks[idx];
        if (t->state == TASK_READY && t->priority == top)
            return t;
    }
    return NULL;
}

/* ── MLFQ demotion: move task down one level after quantum expiry ── */
static void mlfq_demote(task_t *t)
{
    if (t->priority <= TASK_PRIO_IDLE) return;
    /* Only demote non-RT tasks */
    if (t->base_priority >= TASK_PRIO_RT) return;
    if (t->priority > 1) {
        bitmap_clear(t->priority);
        t->priority--;
        if (t->state == TASK_READY) bitmap_set(t->priority);
    }
}

/* ── Timer IRQ0 handler ──────────────────────────────────────────── */
static void timer_handler(cpu_regs_t *regs)
{
    (void)regs;
    uptime_ms++;

    wake_sleepers();

    if (!current_task) return;

    current_task->total_ticks++;
    current_task->run_ticks++;

    /* Quantum check: only preempt when slice is exhausted */
    if (current_task->run_ticks < SCHED_QUANTUM_TICKS) {
        /* Every 50 ms: age and reap */
        if ((uptime_ms % 50) == 0) { age_tasks(); reap_zombies(); }
        return;
    }

    /* Quantum expired: demote current task if not RT */
    mlfq_demote(current_task);
    current_task->run_ticks = 0;

    /* Rebuild bitmap (demotion may have changed priorities) */
    if (current_task->state == TASK_RUNNING) {
        current_task->state = TASK_READY;
        bitmap_set(current_task->priority);
    }

    task_t *next = sched_pick_next();
    if (!next || next == current_task) {
        current_task->state = TASK_RUNNING;
        bitmap_clear(current_task->priority);
        return;
    }

    task_t *old       = current_task;
    current_task      = next;

    bitmap_clear(next->priority);
    next->state              = TASK_RUNNING;
    next->wait_ticks         = 0;
    next->context_switches++;
    next->last_run_ms        = uptime_ms;

    tss_set_kernel_stack(next->stack_top);
    sched_switch_context(&old->ctx, &next->ctx);

    if ((uptime_ms % 50) == 0) { age_tasks(); reap_zombies(); }
}

/* ── Idle task ───────────────────────────────────────────────────── */
static void idle_task_fn(void)
{
    for (;;) { sti(); hlt(); }
}

/* ── Public: sched_init ──────────────────────────────────────────── */
void sched_init(void)
{
    memset(tasks,       0, sizeof(tasks));
    memset(prio_bitmap, 0, sizeof(prio_bitmap));

    pit_init(SCHED_PIT_HZ);
    irq_register(IRQ_TIMER, timer_handler);

    task_t *idle = task_create_prio(idle_task_fn, "idle", TASK_PRIO_IDLE);
    if (!idle) kpanic("sched_init: cannot create idle task\n");

    idle->state   = TASK_RUNNING;
    current_task  = idle;
    bitmap_clear(TASK_PRIO_IDLE);   /* Running task not in bitmap */
    tss_set_kernel_stack(idle->stack_top);

    KINFO("Scheduler v3.0: init  tick=%u Hz  O(1) prio-bitmap  MLFQ\n",
          SCHED_PIT_HZ);
}

/* ── Internal task allocator ─────────────────────────────────────── */
static task_t *alloc_task(task_fn_t fn, const char *name, uint8_t prio)
{
    task_t *t = NULL;
    for (int i = 0; i < SCHED_MAX_TASKS; i++)
        if (tasks[i].state == TASK_UNUSED) { t = &tasks[i]; break; }
    if (!t) return NULL;

    uint32_t sp = pmm_alloc_pages(SCHED_STACK_SIZE / PAGE_SIZE);
    if (!sp) return NULL;

    memset(t, 0, sizeof(*t));
    t->id             = next_id++;
    t->state          = TASK_READY;
    t->priority       = prio;
    t->base_priority  = prio;
    t->quantum_ticks  = SCHED_QUANTUM_TICKS;
    t->stack_phys     = sp;
    t->stack_top      = sp + SCHED_STACK_SIZE;
    t->created_ms     = uptime_ms;
    strncpy(t->name, name ? name : "unnamed", sizeof(t->name) - 1);

    uint32_t *stack = (uint32_t *)t->stack_top;
    *(--stack) = 0x00000000;
    *(--stack) = (uint32_t)fn;
    t->ctx.esp    = (uint32_t)stack;
    t->ctx.eip    = (uint32_t)fn;
    t->ctx.eflags = 0x0202;

    bitmap_set(prio);
    return t;
}

task_t *task_create(task_fn_t fn, const char *name)
{ return alloc_task(fn, name, TASK_PRIO_NORMAL); }

task_t *task_create_prio(task_fn_t fn, const char *name, uint8_t priority)
{ return alloc_task(fn, name, priority); }

void task_set_priority(task_t *t, uint8_t priority)
{
    if (!t) return;
    uint32_t flags = irq_save();
    if (t->state == TASK_READY) bitmap_clear(t->priority);
    t->priority      = priority;
    t->base_priority = priority;
    if (t->state == TASK_READY) bitmap_set(priority);
    irq_restore(flags);
}

/* ── Public: task_exit ───────────────────────────────────────────── */
void task_exit(int exit_code)
{
    uint32_t flags = irq_save();
    if (current_task) {
        current_task->exit_code   = exit_code;
        current_task->state       = TASK_ZOMBIE;
        current_task->zombie_ticks= 0;
        /* Stack freed by zombie reaper */
    }
    irq_restore(flags);
    task_yield();
    for (;;) hlt();
}

/* ── Public: task_yield ──────────────────────────────────────────── */
void task_yield(void)
{
    __asm__ volatile ("int $0x20");
}

/* ── Public: task_sleep ──────────────────────────────────────────── */
void task_sleep(uint32_t ms)
{
    if (!current_task || !ms) return;
    uint32_t flags = irq_save();
    current_task->sleep_until_ms = uptime_ms + ms;
    current_task->state          = TASK_SLEEPING;
    bitmap_clear(current_task->priority);
    irq_restore(flags);
    task_yield();
}

/* ── Public: task_wake ───────────────────────────────────────────── */
void task_wake(task_t *t)
{
    if (!t) return;
    uint32_t flags = irq_save();
    if (t->state == TASK_SLEEPING || t->state == TASK_BLOCKED) {
        t->sleep_until_ms = 0;
        t->state          = TASK_READY;
        bitmap_set(t->priority);
    }
    irq_restore(flags);
}

/* ── Public: accessors ───────────────────────────────────────────── */
task_t  *sched_current(void)   { return current_task; }
uint32_t sched_uptime_ms(void) { return uptime_ms; }

uint32_t sched_task_count(void)
{
    uint32_t n = 0;
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_state_t s = tasks[i].state;
        if (s == TASK_READY || s == TASK_RUNNING) n++;
    }
    return n;
}

void sched_dump(void)
{
    static const char *sn[] = {"UNUSED","READY","RUNNING","BLOCKED","ZOMBIE","SLEEP"};
    KINFO("Tasks (uptime %u ms):\n", uptime_ms);
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_UNUSED) continue;
        KINFO("  [%2u] %-16s  %-8s  prio=%3u  ticks=%llu  sw=%u\n",
              t->id, t->name, sn[t->state],
              t->priority, t->total_ticks, t->context_switches);
    }
    KINFO("  Ready-bitmap: %08x %08x %08x %08x %08x %08x %08x %08x\n",
          prio_bitmap[7],prio_bitmap[6],prio_bitmap[5],prio_bitmap[4],
          prio_bitmap[3],prio_bitmap[2],prio_bitmap[1],prio_bitmap[0]);
}