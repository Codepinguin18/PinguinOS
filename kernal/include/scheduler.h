/**
 * @file scheduler.h
 * @brief Round-Robin-Scheduler (präemptiv) für PinguinOS.
 *
 * Jeder Task ist ein leichtgewichtiger Kernel-Thread. Kontextwechsel finden
 * beim Timer-IRQ (IRQ0, d.h. der PIT) statt. Tasks können in einem der
 * folgenden Zustände sein:
 *
 *   TASK_READY    – kann eingeplant werden
 *   TASK_RUNNING  – wird aktuell ausgeführt
 *   TASK_BLOCKED  – wartet auf ein Ereignis (Sleep, I/O …)
 *   TASK_ZOMBIE   – beendet, wartet auf Bereinigung
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "idt.h"

/* ── Konfiguration ───────────────────────────────────────────────── */
#define SCHED_MAX_TASKS    64      /* Maximale Anzahl gleichzeitiger Tasks */
#define SCHED_STACK_SIZE   8192   /* Standard-Kernel-Thread-Stack (8 KB)  */
#define SCHED_QUANTUM_MS   10     /* Zeitscheibe in Millisekunden         */

/* ── Task-Zustände ───────────────────────────────────────────────── */
typedef enum task_state {
    TASK_UNUSED  = 0,
    TASK_READY   = 1,
    TASK_RUNNING = 2,
    TASK_BLOCKED = 3,
    TASK_ZOMBIE  = 4,
} task_state_t;

/* ── Gespeicherter CPU-Kontext (muss zum Layout in isr.S passen) ─── */
typedef struct task_context {
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t eip;
    uint32_t eflags;
} task_context_t;

/* ── Task Control Block (TCB) ────────────────────────────────────── */
typedef struct task {
    uint32_t      id;              /* Eindeutige Task-ID               */
    task_state_t  state;
    task_context_t ctx;            /* Gespeicherter Register-Status    */
    uint32_t      stack_top;       /* Virtuelle Adresse der Stack-Oberkante */
    uint32_t      stack_phys;      /* Physische Adresse der Stack-Seiten */
    uint32_t      sleep_until_ms;  /* Weckzeit (wenn BLOCKED)          */
    char          name[32];        /* Menschenlesbarer Task-Name       */
    int           exit_code;
} task_t;

/** Signatur für den Einstiegspunkt eines Kernel-Tasks. */
typedef void (*task_fn_t)(void);

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief Initialisiert den Scheduler und erstellt den Idle-Task.
 *
 * Muss nach pmm_init() und paging_init() aufgerufen werden.
 * Nutzt IRQ0 (Timer) für Präemption.
 */
void sched_init(void);

/**
 * @brief Erstellt einen neuen Kernel-Task.
 * @param fn    Einstiegsfunktion.
 * @param name  Menschenlesbarer Task-Name (max. 31 Zeichen).
 * @return Zeiger auf den TCB des neuen Tasks, oder NULL im Fehlerfall.
 */
task_t *task_create(task_fn_t fn, const char *name);

/**
 * @brief Beendet den aktuellen Task.
 * @param exit_code  Exit-Statuscode.
 *
 * Diese Funktion kehrt nicht zurück.
 */
void NORETURN task_exit(int exit_code);

/** Gibt die CPU sofort an den nächsten bereiten Task ab. */
void task_yield(void);

/**
 * @brief Blockiert den aktuellen Task für mindestens @p ms Millisekunden.
 * @param ms  Schlafdauer in Millisekunden.
 */
void task_sleep(uint32_t ms);

/**
 * @brief Weckt einen blockierten Task.
 * @param t  Zu weckender Task (muss im Zustand TASK_BLOCKED sein).
 */
void task_wake(task_t *t);

/** @return Zeiger auf den TCB des aktuell laufenden Tasks. */
task_t *sched_current(void);

/** @return Anzahl der Tasks im Zustand TASK_READY oder TASK_RUNNING. */
uint32_t sched_task_count(void);

/** @return Millisekunden seit dem Start des Kernel-Timers. */
uint32_t sched_uptime_ms(void);

/** Gibt eine Tabelle aller Tasks im Serial-Log aus. */
void sched_dump(void);

#endif /* _SCHEDULER_H */
