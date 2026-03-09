/**
 * @file kprobes.h
 * @brief Kprobes API – Feature #97.
 */
#ifndef _KPROBES_H
#define _KPROBES_H

#include "types.h"
#include "idt.h"

typedef void (*kprobe_handler_t)(uint32_t addr, cpu_regs_t *regs);

void kprobes_init(void);
int  kprobe_register(uint32_t addr, kprobe_handler_t pre, kprobe_handler_t post);
void kprobe_unregister(uint32_t addr);

#endif /* _KPROBES_H */