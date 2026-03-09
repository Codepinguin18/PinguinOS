/**
 * @file kprobes.c
 * @brief Kprobes – kernel dynamic tracing via INT3 – Feature #97.
 *
 * A kprobe inserts an INT3 (0xCC) breakpoint at a kernel address.
 * When the CPU executes it, the debug exception (vector 3) fires and
 * calls the registered handler (pre_handler).  Execution is then resumed
 * via single-step (TF flag) and post_handler is called after the
 * original instruction completes.
 *
 * API:
 *   kprobe_register(addr, pre_handler, post_handler)  – arm a probe
 *   kprobe_unregister(addr)                           – remove a probe
 */

#include "../include/kprobes.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/sync.h"

/* ── Probe table ─────────────────────────────────────────────────── */
#define KPROBE_MAX  32

typedef struct kprobe {
    uint32_t addr;
    uint8_t  saved_byte;    /* Original byte replaced by 0xCC */
    kprobe_handler_t pre;
    kprobe_handler_t post;
    bool     active;
    bool     single_stepping;
} kprobe_t;

static kprobe_t  probes[KPROBE_MAX];
static spinlock_t kp_lock = SPINLOCK_INIT;

/* ── Find probe by address ───────────────────────────────────────── */
static kprobe_t *find_probe(uint32_t addr)
{
    for (int i = 0; i < KPROBE_MAX; i++)
        if (probes[i].active && probes[i].addr == addr) return &probes[i];
    return NULL;
}

/* ── INT3 handler (vector 3) ─────────────────────────────────────── */
static void kprobe_int3_handler(cpu_regs_t *regs)
{
    uint32_t hit_addr = regs->eip - 1;   /* EIP points past INT3 */

    kprobe_t *kp = find_probe(hit_addr);
    if (!kp) return;   /* Not ours – let debugger handle */

    /* Call pre-handler */
    if (kp->pre) kp->pre(kp->addr, regs);

    /* Restore original instruction byte */
    *(uint8_t *)kp->addr = kp->saved_byte;

    /* Set up single-step to fire post-handler */
    regs->eflags |= (1u << 8);   /* TF = Trap Flag */
    regs->eip     = kp->addr;    /* Re-execute original instruction */
    kp->single_stepping = true;
}

/* ── Single-step handler (vector 1) ─────────────────────────────── */
static void kprobe_debug_handler(cpu_regs_t *regs)
{
    /* Find which probe was single-stepping */
    for (int i = 0; i < KPROBE_MAX; i++) {
        kprobe_t *kp = &probes[i];
        if (!kp->active || !kp->single_stepping) continue;

        regs->eflags &= ~(1u << 8);   /* Clear TF */
        kp->single_stepping = false;

        /* Re-arm the INT3 */
        *(uint8_t *)kp->addr = 0xCC;

        /* Call post-handler */
        if (kp->post) kp->post(kp->addr, regs);
        return;
    }
}

/* ── Public: kprobes_init ────────────────────────────────────────── */
void kprobes_init(void)
{
    memset(probes, 0, sizeof(probes));
    exception_register(1, kprobe_debug_handler);   /* #DB */
    exception_register(3, kprobe_int3_handler);    /* #BP */
    KINFO("kprobes: initialised (%u slots)\n", KPROBE_MAX);
}

/* ── Public: kprobe_register ─────────────────────────────────────── */
int kprobe_register(uint32_t addr, kprobe_handler_t pre, kprobe_handler_t post)
{
    uint32_t flags = spin_lock_irqsave(&kp_lock);

    if (find_probe(addr)) { spin_unlock_irqrestore(&kp_lock, flags); return -1; }

    kprobe_t *kp = NULL;
    for (int i = 0; i < KPROBE_MAX; i++)
        if (!probes[i].active) { kp = &probes[i]; break; }

    if (!kp) { spin_unlock_irqrestore(&kp_lock, flags); return -1; }

    kp->addr        = addr;
    kp->saved_byte  = *(uint8_t *)addr;
    kp->pre         = pre;
    kp->post        = post;
    kp->active      = true;
    kp->single_stepping = false;

    *(uint8_t *)addr = 0xCC;   /* Arm the probe */

    spin_unlock_irqrestore(&kp_lock, flags);
    KINFO("kprobe: armed at 0x%08x (saved=0x%02x)\n", addr, kp->saved_byte);
    return 0;
}

/* ── Public: kprobe_unregister ───────────────────────────────────── */
void kprobe_unregister(uint32_t addr)
{
    uint32_t flags = spin_lock_irqsave(&kp_lock);
    kprobe_t *kp = find_probe(addr);
    if (kp) {
        *(uint8_t *)kp->addr = kp->saved_byte;   /* Restore original byte */
        kp->active = false;
        KINFO("kprobe: disarmed at 0x%08x\n", addr);
    }
    spin_unlock_irqrestore(&kp_lock, flags);
}