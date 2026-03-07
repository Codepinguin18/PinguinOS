/**
 * @file process.c
 * @brief Prozessverwaltung, VMA-Tracking und Signale für PinguinOS.
 */
#include "../include/process.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/heap.h"
#include "../include/slab.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── Globale Prozesstabelle ──────────────────────────────────────── */
static process_t  proc_table[PROC_MAX];
static process_t *current_proc = NULL;
static uint32_t   next_pid     = 1;

/* ── Hilfsfunktion: Default-Signal-Aktionen ──────────────────────── */
static void sig_default_action(process_t *proc, int sig)
{
    switch (sig) {
    case SIGKILL:
    case SIGTERM:
        proc->state     = PROC_ZOMBIE;
        proc->exit_code = 128 + sig;
        break;
    case SIGSTOP:
        proc->state = PROC_STOPPED;
        break;
    case SIGCONT:
        if (proc->state == PROC_STOPPED)
            proc->state = PROC_READY;
        break;
    case SIGCHLD:
        /* Standardaktion: ignorieren */
        break;
    default:
        proc->state     = PROC_ZOMBIE;
        proc->exit_code = 128 + sig;
        break;
    }
}

/* ── proc_init ───────────────────────────────────────────────────── */
void proc_init(void)
{
    memset(proc_table, 0, sizeof(proc_table));
    current_proc = NULL;
    KINFO("Proc: Prozesstabelle initialisiert (%u Slots)\n", PROC_MAX);
}

/* ── proc_create ─────────────────────────────────────────────────── */
process_t *proc_create(const char *name)
{
    process_t *proc = NULL;
    for (int i = 0; i < PROC_MAX; i++) {
        if (proc_table[i].state == PROC_UNUSED) {
            proc = &proc_table[i];
            break;
        }
    }
    if (!proc) return NULL;

    memset(proc, 0, sizeof(*proc));
    proc->pid   = next_pid++;
    proc->ppid  = current_proc ? current_proc->pid : 0;
    proc->pgid  = proc->pid;
    proc->sid   = proc->pid;
    proc->state = PROC_READY;
    strncpy(proc->name, name ? name : "unknown", PROC_NAME_LEN - 1);

    /* Kernel-Stack anlegen (2 Seiten) */
    proc->kstack_phys = pmm_alloc_pages(2);
    proc->kstack_top  = proc->kstack_phys + 2 * PAGE_SIZE;

    /* Default-Signal-Handler */
    for (int i = 0; i < NSIG; i++)
        proc->sig_handlers[i] = SIG_DFL;

    KINFO("Proc: PID %u '%s' erstellt\n", proc->pid, proc->name);
    return proc;
}

/* ── proc_fork ───────────────────────────────────────────────────── */
process_t *proc_fork(process_t *parent)
{
    if (!parent) return NULL;

    process_t *child = proc_create(parent->name);
    if (!child) return NULL;

    child->ppid = parent->pid;
    child->pgid = parent->pgid;
    child->sid  = parent->sid;

    /* Signal-Handler kopieren */
    memcpy(child->sig_handlers, parent->sig_handlers,
           sizeof(parent->sig_handlers));
    child->sig_mask = parent->sig_mask;

    /* VMA-Liste kopieren (Copy-on-Write: Flags als readonly markieren) */
    vma_t *vma = parent->vma_list;
    while (vma) {
        vma_t *new_vma = vma_insert(child, vma->start, vma->end,
                                     vma->flags & ~VMA_WRITE);
        if (new_vma) {
            new_vma->file_offset = vma->file_offset;
        }
        vma = vma->next;
    }

    /* Neues Page-Directory anlegen und Kernel-Mappings übernehmen */
    uint32_t new_pd = pmm_alloc_page();
    if (!new_pd) { proc_exit(1); return NULL; }
    /* Kernel-PD-Einträge kopieren (obere 1 GB) */
    memcpy((void *)new_pd, (void *)paging_get_cr3(), PAGE_SIZE);
    child->cr3 = new_pd;

    KINFO("Proc: fork() PID %u → PID %u\n", parent->pid, child->pid);
    return child;
}

/* ── Signale ─────────────────────────────────────────────────────── */
void proc_signal(process_t *proc, int sig)
{
    if (!proc || sig <= 0 || sig >= NSIG) return;
    if (proc->sig_mask & (1U << sig)) return;   /* Blockiert */

    proc->sig_pending |= (1U << sig);
}

void proc_set_sighandler(process_t *proc, int sig, sig_handler_t handler)
{
    if (!proc || sig <= 0 || sig >= NSIG) return;
    /* SIGKILL und SIGSTOP können nicht überschrieben werden */
    if (sig == SIGKILL || sig == SIGSTOP) return;
    proc->sig_handlers[sig] = handler;
}

void proc_handle_signals(process_t *proc)
{
    if (!proc || !proc->sig_pending) return;

    for (int sig = 1; sig < NSIG; sig++) {
        if (!(proc->sig_pending & (1U << sig))) continue;
        proc->sig_pending &= ~(1U << sig);

        sig_handler_t handler = proc->sig_handlers[sig];
        if (handler == SIG_IGN) continue;
        if (handler == SIG_DFL) {
            sig_default_action(proc, sig);
        } else {
            handler(sig);
        }
    }
}

/* ── VMA-Operationen ─────────────────────────────────────────────── */
vma_t *vma_find(process_t *proc, uint32_t addr)
{
    vma_t *v = proc->vma_list;
    while (v) {
        if (addr >= v->start && addr < v->end) return v;
        v = v->next;
    }
    return NULL;
}

vma_t *vma_insert(process_t *proc, uint32_t start, uint32_t end, uint32_t flags)
{
    vma_t *v = (vma_t *)slab_alloc(slab_cache_vma);
    if (!v) return NULL;

    v->start       = ALIGN_DOWN(start, PAGE_SIZE);
    v->end         = ALIGN_UP(end,     PAGE_SIZE);
    v->flags       = flags;
    v->file_offset = 0;

    /* Einsortieren (nach Adresse aufsteigend) */
    vma_t **pp = &proc->vma_list;
    while (*pp && (*pp)->start < v->start) pp = &(*pp)->next;
    v->next = *pp;
    *pp     = v;

    return v;
}

void vma_remove(process_t *proc, vma_t *vma)
{
    vma_t **pp = &proc->vma_list;
    while (*pp && *pp != vma) pp = &(*pp)->next;
    if (*pp) {
        *pp = vma->next;
        slab_free(slab_cache_vma, vma);
    }
}

void vma_dump(process_t *proc)
{
    KINFO("VMA-Liste für PID %u:\n", proc->pid);
    vma_t *v = proc->vma_list;
    while (v) {
        KINFO("  0x%08x – 0x%08x  [%c%c%c%c]\n",
              v->start, v->end,
              (v->flags & VMA_READ)   ? 'r' : '-',
              (v->flags & VMA_WRITE)  ? 'w' : '-',
              (v->flags & VMA_EXEC)   ? 'x' : '-',
              (v->flags & VMA_SHARED) ? 's' : 'p');
        v = v->next;
    }
}

process_t *proc_find(uint32_t pid)
{
    for (int i = 0; i < PROC_MAX; i++)
        if (proc_table[i].pid == pid && proc_table[i].state != PROC_UNUSED)
            return &proc_table[i];
    return NULL;
}

process_t *proc_current(void) { return current_proc; }

void proc_exit(int code)
{
    if (current_proc) {
        current_proc->exit_code = code;
        current_proc->state     = PROC_ZOMBIE;
        /* Eltern-Prozess benachrichtigen */
        process_t *parent = proc_find(current_proc->ppid);
        if (parent) proc_signal(parent, SIGCHLD);
    }
    for (;;) { cli(); hlt(); }
}
