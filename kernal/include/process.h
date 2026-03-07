/**
 * @file process.h
 * @brief Prozess-Deskriptor, VMA-Tracking, Signale für PinguinOS.
 *
 * Features #6 (VMA), #11 (Fork), #12 (Signale), #17 (Process Groups)
 */
#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include "paging.h"

/* ── Signalnummern (POSIX-kompatibel) ────────────────────────────── */
#define SIGKILL   9
#define SIGTERM  15
#define SIGCHLD  17
#define SIGSTOP  19
#define SIGCONT  18
#define SIGINT    2
#define SIGALRM  14
#define SIGUSR1  10
#define SIGUSR2  12
#define NSIG     32

/* Signal-Handler-Konstanten */
#define SIG_DFL  ((sig_handler_t)0)    /* Standardaktion         */
#define SIG_IGN  ((sig_handler_t)1)    /* Signal ignorieren      */

typedef void (*sig_handler_t)(int sig);

/* ── VMA (Virtual Memory Area) ─────────────────────────────────── */
#define VMA_READ    (1 << 0)
#define VMA_WRITE   (1 << 1)
#define VMA_EXEC    (1 << 2)
#define VMA_SHARED  (1 << 3)
#define VMA_ANON    (1 << 4)   /* Anonym (kein Datei-Backing)  */
#define VMA_STACK   (1 << 5)

typedef struct vma {
    uint32_t     start;        /* Virtuelle Startadresse (inkl.) */
    uint32_t     end;          /* Virtuelle Endadresse (exkl.)   */
    uint32_t     flags;        /* VMA_* Flags                    */
    uint32_t     file_offset;  /* Offset ins Backing-File        */
    struct vma  *next;         /* Nächste VMA in der Liste       */
} vma_t;

/* ── Prozess-Zustände ────────────────────────────────────────────── */
typedef enum proc_state {
    PROC_UNUSED  = 0,
    PROC_RUNNING = 1,
    PROC_READY   = 2,
    PROC_BLOCKED = 3,
    PROC_ZOMBIE  = 4,
    PROC_STOPPED = 5,
} proc_state_t;

/* ── Prozess-Kontrollblock (PCB) ──────────────────────────────────  */
#define PROC_MAX        64
#define PROC_NAME_LEN   32
#define PROC_FD_MAX     32     /* Max gleichzeitig offene Dateien  */

typedef struct process {
    uint32_t      pid;
    uint32_t      ppid;           /* Eltern-Prozess                 */
    uint32_t      pgid;           /* Process-Group-ID               */
    uint32_t      sid;            /* Session-ID                     */
    proc_state_t  state;
    int           exit_code;

    /* Adressraum */
    uint32_t      cr3;            /* Physische Adresse des Page Dir */
    vma_t        *vma_list;       /* Verkettete VMA-Liste           */

    /* Signale */
    uint32_t      sig_pending;    /* Maske ausstehender Signale     */
    uint32_t      sig_mask;       /* Blockierte Signale             */
    sig_handler_t sig_handlers[NSIG];

    /* Kernel-Stack (für den zugehörigen Kernel-Thread) */
    uint32_t      kstack_top;
    uint32_t      kstack_phys;

    /* User-Stack */
    uint32_t      ustack_top;

    /* Dateideskriptoren (NULL = nicht offen) */
    void         *fds[PROC_FD_MAX];

    /* CPU-Nutzungsstatistik */
    uint64_t      utime_ticks;    /* User-Mode-Ticks */
    uint64_t      ktime_ticks;    /* Kernel-Mode-Ticks */

    char          name[PROC_NAME_LEN];
} process_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

void      proc_init(void);

/** Erstellt einen neuen Prozess (leere Hülle). */
process_t *proc_create(const char *name);

/** Fork: dupliziert den aktuellen Prozess. */
process_t *proc_fork(process_t *parent);

/** Signiert einen Prozess. */
void       proc_signal(process_t *proc, int sig);

/** Signal-Handler registrieren. */
void       proc_set_sighandler(process_t *proc, int sig, sig_handler_t handler);

/** Ausstehende Signale abarbeiten. */
void       proc_handle_signals(process_t *proc);

/** Aktuellen Prozess beenden. */
void       proc_exit(int code);

/** Prozess nach PID suchen. */
process_t *proc_find(uint32_t pid);

/** Aktuellen Prozess zurückgeben. */
process_t *proc_current(void);

/* VMA-Operationen */
vma_t *vma_find(process_t *proc, uint32_t addr);
vma_t *vma_insert(process_t *proc, uint32_t start, uint32_t end, uint32_t flags);
void   vma_remove(process_t *proc, vma_t *vma);
void   vma_dump(process_t *proc);

#endif /* _PROCESS_H */
