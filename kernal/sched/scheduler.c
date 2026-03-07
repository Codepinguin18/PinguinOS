/**
 * @file scheduler.c
 * @brief Prioritätsbasierter präemptiver Scheduler für PinguinOS.
 *
 * Kontextwechsel werden durch IRQ0 (PIT, ~100 Hz) gesteuert.
 * Jeder Task hat seinen eigenen 8 KB Kernel-Stack.
 *
 * Feature #13 – Prioritäts-Scheduling:
 *   sched_next() wählt immer den bereitstehenden (READY) Task mit der höchsten Priorität.
 *   Tasks gleicher Priorität teilen sich die CPU via Round-Robin (Zeitschlitz =
 *   SCHED_QUANTUM_MS). Ein einfacher Aging-Mechanismus verhindert Verhungern (Starvation):
 *   Tasks, die ≥ AGING_THRESHOLD Ticks gewartet haben, erhalten einen
 *   Prioritäts-Boost um einen Tick.
 */

#include "../include/scheduler.h"
#include "../include/idt.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/gdt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── Externer Kontextwechsel (Assembly) ─────────────────────────── */
extern void sched_switch_context(task_context_t *old_ctx, task_context_t *new_ctx);

/* ── PIT-Konfiguration ───────────────────────────────────────────── */
#define PIT_FREQ_HZ    100        /* Tick-Rate                       */
#define PIT_BASE_FREQ  1193182    /* PIT-Eingangsfrequenz in Hz      */
#define PIT_CMD_PORT   0x43
#define PIT_CH0_PORT   0x40

/* Ticks ohne Ausführung, bevor die Priorität erhöht wird (+1, gedeckelt bei 254) */
#define AGING_THRESHOLD  50

/* Modulzustand */
static task_t   tasks[SCHED_MAX_TASKS];
static task_t  *current_task = NULL;
static uint32_t next_id      = 1;
static uint32_t uptime_ticks = 0;

/* ── PIT-Einrichtung ─────────────────────────────────────────────── */
static void pit_init(uint32_t hz)
{
    uint32_t divisor = PIT_BASE_FREQ / hz;
    outb(PIT_CMD_PORT, 0x36);
    outb(PIT_CH0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CH0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

/* ── Prioritäts-Scheduling: Besten nächsten Task finden ──────────── */
static task_t *sched_next(void)
{
    uint32_t now_ms = uptime_ticks * (1000 / PIT_FREQ_HZ);

    /* 1. Schlafende Tasks aufwecken, deren Timer abgelaufen ist */
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_BLOCKED && t->sleep_until_ms <= now_ms)
            t->state = TASK_READY;
    }

    /* 2. Aging: Priorität von lange wartenden Tasks erhöhen (Starvation verhindern) */
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state != TASK_READY) continue;
        t->wait_ticks++;
        if (t->wait_ticks >= AGING_THRESHOLD) {
            t->wait_ticks = 0;
            if (t->priority < 254)
                t->priority++;   /* Temporärer Boost; wird bei Ausführung zurückgesetzt */
        }
    }

    /* 3. Den READY-Task mit der höchsten Priorität finden.
     *    Bei gleicher Priorität wird der Task nach dem aktuellen bevorzugt
     *    (Round-Robin Entscheidungsfindung). */
    int      start     = current_task ? (int)(current_task - tasks) : -1;
    task_t  *best      = NULL;
    int      best_idx  = -1;

    for (int i = 1; i <= SCHED_MAX_TASKS; i++) {
        int idx = (start + i) % SCHED_MAX_TASKS;
        task_t *t = &tasks[idx];
        if (t->state != TASK_READY && t->state != TASK_RUNNING) continue;
        if (!best || t->priority > best->priority) {
            best     = t;
            best_idx = idx;
        }
    }
    (void)best_idx;
    return best;
}

/* ── Timer-IRQ-Handler ───────────────────────────────────────────── */
static void timer_handler(cpu_regs_t *regs)
{
    (void)regs;
    uptime_ticks++;

    if (!current_task) return;

    /* Quantums-Ticks des aktuellen Tasks verringern */
    if (current_task->quantum_ticks > 0)
        current_task->quantum_ticks--;

    /* Nur unterbrechen, wenn das Quantum verbraucht ist */
    if (current_task->quantum_ticks > 0) return;

    task_t *next = sched_next();
    if (!next || next == current_task) {
        /* Quantum auffüllen und fortfahren */
        current_task->quantum_ticks = SCHED_QUANTUM_MS / (1000 / PIT_FREQ_HZ);
        return;
    }

    task_t *old = current_task;
    current_task = next;

    old->state  = (old->state == TASK_RUNNING) ? TASK_READY : old->state;
    next->state = TASK_RUNNING;

    /* Quantum für den neuen Task auffüllen */
    next->quantum_ticks = SCHED_QUANTUM_MS / (1000 / PIT_FREQ_HZ);
    next->run_ticks++;

    /* Basis-Priorität wiederherstellen, nachdem der Aging-Boost "verbraucht" wurde */
    if (next->priority > next->base_priority)
        next->priority = next->base_priority;
    next->wait_ticks = 0;

    tss_set_kernel_stack(next->stack_top);
    sched_switch_context(&old->ctx, &next->ctx);
}

/* ── Idle-Task ───────────────────────────────────────────────────── */
static void idle_task_fn(void)
{
    for (;;) { sti(); hlt(); }
}

/* ── Öffentlich: sched_init ──────────────────────────────────────── */
void sched_init(void)
{
    memset(tasks, 0, sizeof(tasks));

    pit_init(PIT_FREQ_HZ);
    irq_register(IRQ_TIMER, timer_handler);

    task_t *idle = task_create_prio(idle_task_fn, "idle", TASK_PRIO_IDLE);
    if (!idle) kpanic("sched_init: kann Idle-Task nicht erstellen\n");

    idle->state = TASK_RUNNING;
    current_task = idle;
    tss_set_kernel_stack(idle->stack_top);

    KINFO("Scheduler: initialisiert (Tick=%u Hz, Prioritäts-Scheduling AN)\n",
          PIT_FREQ_HZ);
}

/* ── Intern: Gemeinsame Task-Initialisierung ─────────────────────── */
static task_t *task_alloc(task_fn_t fn, const char *name, uint8_t priority)
{
    task_t *t = NULL;
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (tasks[i].state == TASK_UNUSED) { t = &tasks[i]; break; }
    }
    if (!t) return NULL;

    uint32_t stack_pages = SCHED_STACK_SIZE / PAGE_SIZE;
    uint32_t stack_phys  = pmm_alloc_pages(stack_pages);
    if (!stack_phys) return NULL;

    memset(t, 0, sizeof(*t));
    t->id            = next_id++;
    t->state         = TASK_READY;
    t->stack_phys    = stack_phys;
    t->stack_top     = stack_phys + SCHED_STACK_SIZE;
    t->priority      = priority;
    t->base_priority = priority;
    t->quantum_ticks = SCHED_QUANTUM_MS / (1000 / PIT_FREQ_HZ);
    strncpy(t->name, name ? name : "unnamed", sizeof(t->name) - 1);

    uint32_t *stack = (uint32_t *)t->stack_top;
    *(--stack) = 0x00000000;
    *(--stack) = (uint32_t)fn;

    t->ctx.esp    = (uint32_t)stack;
    t->ctx.eip    = (uint32_t)fn;
    t->ctx.eflags = 0x0202;

    return t;
}

/* ── Öffentlich: task_create ─────────────────────────────────────── */
task_t *task_create(task_fn_t fn, const char *name)
{
    return task_alloc(fn, name, TASK_PRIO_NORMAL);
}

/* ── Öffentlich: task_create_prio ────────────────────────────────── */
task_t *task_create_prio(task_fn_t fn, const char *name, uint8_t priority)
{
    return task_alloc(fn, name, priority);
}

/* ── Öffentlich: task_set_priority ───────────────────────────────── */
void task_set_priority(task_t *t, uint8_t priority)
{
    if (!t) t = current_task;
    if (!t) return;
    t->base_priority = priority;
    t->priority      = priority;
}

/* ── Öffentlich: task_exit ───────────────────────────────────────── */
void task_exit(int exit_code)
{
    uint32_t flags = irq_save();
    if (current_task) {
        current_task->exit_code = exit_code;
        current_task->state     = TASK_ZOMBIE;
        pmm_free_pages(current_task->stack_phys, SCHED_STACK_SIZE / PAGE_SIZE);
    }
    irq_restore(flags);
    task_yield();
    for (;;) hlt();
}

/* ── Öffentlich: task_yield ──────────────────────────────────────── */
void task_yield(void)
{
    __asm__ volatile ("int $0x20");
}

/* ── Öffentlich: task_sleep ──────────────────────────────────────── */
void task_sleep(uint32_t ms)
{
    if (!current_task) return;
    uint32_t flags = irq_save();
    uint32_t now_ms = uptime_ticks * (1000 / PIT_FREQ_HZ);
    current_task->sleep_until_ms = now_ms + ms;
    current_task->state = TASK_BLOCKED;
    irq_restore(flags);
    task_yield();
}

/* ── Öffentlich: task_wake ───────────────────────────────────────── */
void task_wake(task_t *t)
{
    if (t && t->state == TASK_BLOCKED) {
        t->sleep_until_ms = 0;
        t->state = TASK_READY;
    }
}

/* ── Öffentlich: Zugriffsmethoden ────────────────────────────────── */
task_t  *sched_current(void)   { return current_task; }
uint32_t sched_uptime_ms(void) { return uptime_ticks * (1000 / PIT_FREQ_HZ); }

uint32_t sched_task_count(void)
{
    uint32_t count = 0;
    for (int i = 0; i < SCHED_MAX_TASKS; i++)
        if (tasks[i].state == TASK_READY || tasks[i].state == TASK_RUNNING)
            count++;
    return count;
}

void sched_dump(void)
{
    static const char *state_names[] = {
        "UNUSED", "READY", "RUNNING", "BLOCKED", "ZOMBIE"
    };
    KINFO("Task-Liste (Uptime %u ms):\n", sched_uptime_ms());
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_UNUSED) continue;
        KINFO("  [%2u] %-16s  %s  prio=%3u  esp=0x%08x  ticks=%u\n",
              t->id, t->name, state_names[t->state],
              t->priority, t->ctx.esp, t->run_ticks);
    }
}