/**
 * @file scheduler.c
 * @brief Primitiver Round-Robin-Scheduler für PinguinOS.
 *
 * Kontextwechsel werden durch IRQ0 (PIT, ~100 Hz) ausgelöst.
 * Jeder Task besitzt seinen eigenen 8 KB Kernel-Stack.
 */

#include "../include/scheduler.h"
#include "../include/idt.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/gdt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── Externer Assembler-Kontextwechsel ──────────────────────────── */
extern void sched_switch_context(task_context_t *old_ctx, task_context_t *new_ctx);

/* ── PIT-Konfiguration ───────────────────────────────────────────── */
#define PIT_FREQ_HZ    100        /* Tick-Rate                       */
#define PIT_BASE_FREQ  1193182   /* PIT-Eingangsfrequenz in Hz      */
#define PIT_CMD_PORT   0x43
#define PIT_CH0_PORT   0x40

/* ── Modul-Status ────────────────────────────────────────────────── */
static task_t   tasks[SCHED_MAX_TASKS];
static task_t  *current_task = NULL;
static uint32_t next_id      = 1;
static uint32_t uptime_ticks = 0;    /* Timer-Ticks seit Systemstart  */

/* ── PIT-Setup ───────────────────────────────────────────────────── */
static void pit_init(uint32_t hz)
{
    uint32_t divisor = PIT_BASE_FREQ / hz;
    outb(PIT_CMD_PORT, 0x36);                    /* Kanal 0, Lowbyte/Highbyte, Modus 3 */
    outb(PIT_CH0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CH0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

/* ── Den nächsten bereiten Task finden ────────────────────────────── */
static task_t *sched_next(void)
{
    /* Schlafende Tasks wecken, deren Zeit abgelaufen ist */
    uint32_t now_ms = uptime_ticks * (1000 / PIT_FREQ_HZ);
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_BLOCKED && t->sleep_until_ms <= now_ms)
            t->state = TASK_READY;
    }

    /* Round-Robin ab dem Task nach dem aktuellen */
    int start = current_task ? (int)(current_task - tasks) : -1;
    for (int i = 1; i <= SCHED_MAX_TASKS; i++) {
        int idx = (start + i) % SCHED_MAX_TASKS;
        if (tasks[idx].state == TASK_READY || tasks[idx].state == TASK_RUNNING)
            return &tasks[idx];
    }
    return NULL;
}

/* ── Timer IRQ Handler ───────────────────────────────────────────── */
static void timer_handler(cpu_regs_t *regs)
{
    (void)regs;
    uptime_ticks++;

    /* Nur planen, wenn der Scheduler läuft */
    if (!current_task) return;

    task_t *next = sched_next();
    if (!next || next == current_task) return;

    task_t *old = current_task;
    current_task = next;

    old->state  = (old->state == TASK_RUNNING) ? TASK_READY : old->state;
    next->state = TASK_RUNNING;

    /* Den Kernel-Stack im TSS für den neuen Task aktualisieren */
    tss_set_kernel_stack(next->stack_top);

    sched_switch_context(&old->ctx, &next->ctx);
}

/* ── Idle-Task: Läuft, wenn nichts anderes bereit ist ────────────── */
static void idle_task_fn(void)
{
    for (;;) {
        sti();
        hlt();
    }
}

/* ── Öffentlich: sched_init ──────────────────────────────────────── */
void sched_init(void)
{
    memset(tasks, 0, sizeof(tasks));

    /* PIT konfigurieren */
    pit_init(PIT_FREQ_HZ);
    irq_register(IRQ_TIMER, timer_handler);

    /* Idle-Task erstellen */
    task_t *idle = task_create(idle_task_fn, "idle");
    if (!idle) kpanic("sched_init: Idle-Task konnte nicht erstellt werden\n");

    /* Der Idle-Task ist sofort der aktuelle Task */
    idle->state = TASK_RUNNING;
    current_task = idle;
    tss_set_kernel_stack(idle->stack_top);

    KINFO("Scheduler: initialisiert  (Tick=%u Hz)\n", PIT_FREQ_HZ);
}

/* ── Öffentlich: task_create ─────────────────────────────────────── */
task_t *task_create(task_fn_t fn, const char *name)
{
    /* Freien Slot suchen */
    task_t *t = NULL;
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (tasks[i].state == TASK_UNUSED) { t = &tasks[i]; break; }
    }
    if (!t) return NULL;

    /* Stack allokieren (2 Pages = 8 KB) */
    uint32_t stack_pages = SCHED_STACK_SIZE / PAGE_SIZE;
    uint32_t stack_phys  = pmm_alloc_pages(stack_pages);
    if (!stack_phys) return NULL;

    memset(t, 0, sizeof(*t));
    t->id         = next_id++;
    t->state      = TASK_READY;
    t->stack_phys = stack_phys;
    t->stack_top  = stack_phys + SCHED_STACK_SIZE;
    strncpy(t->name, name ? name : "unnamed", sizeof(t->name) - 1);

    /*
     * Initialen Kontext so aufbauen, dass wenn sched_switch_context
     * in den neuen Task "zurückkehrt", die Ausführung bei @fn beginnt.
     *
     * Das Sichern/Wiederherstellen des Kontextes in isr.S erwartet die
     * Rücksprungadresse auf dem Stack des Tasks. Wir pushen dort die
     * Adresse von fn.
     */
    uint32_t *stack = (uint32_t *)t->stack_top;
    *(--stack) = 0x00000000;   /* Fake Rücksprungadresse (task_exit) */
    *(--stack) = (uint32_t)fn; /* EIP für den ersten Sprung beim Wechsel */

    t->ctx.esp    = (uint32_t)stack;
    t->ctx.eip    = (uint32_t)fn;
    t->ctx.eflags = 0x0202;   /* IF=1, reserviertes Bit 1 gesetzt      */

    return t;
}

/* ── Öffentlich: task_exit ───────────────────────────────────────── */
void task_exit(int exit_code)
{
    uint32_t flags = irq_save();
    if (current_task) {
        current_task->exit_code = exit_code;
        current_task->state     = TASK_ZOMBIE;
        pmm_free_pages(current_task->stack_phys,
                       SCHED_STACK_SIZE / PAGE_SIZE);
    }
    irq_restore(flags);

    /* Rechenzeit abgeben; der Scheduler wird uns nie wieder wählen (ZOMBIE) */
    task_yield();
    for (;;) hlt();   /* Sollte nie erreicht werden */
}

/* ── Öffentlich: task_yield ──────────────────────────────────────── */
void task_yield(void)
{
    /* Software-Kontextwechsel auslösen durch Aufruf der Timer-Logik */
    __asm__ volatile ("int $0x20");   /* Vector 32 = IRQ0 */
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

/* ── Öffentlich: Accessoren ──────────────────────────────────────── */
task_t  *sched_current(void)     { return current_task; }
uint32_t sched_uptime_ms(void)   { return uptime_ticks * (1000 / PIT_FREQ_HZ); }

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
    KINFO("Task-Liste  (Uptime %u ms):\n", sched_uptime_ms());
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        task_t *t = &tasks[i];
        if (t->state == TASK_UNUSED) continue;
        KINFO("  [%2u] %-16s  %s  esp=0x%08x\n",
              t->id, t->name, state_names[t->state], t->ctx.esp);
    }
}
