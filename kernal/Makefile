# ============================================================
#  PinguinOS Kernel  –  Makefile v2.0
#  Targets:  all   iso   run   run-kernel   debug   clean
# ============================================================

CROSS   ?= i686-elf
CC       = $(CROSS)-gcc
AS       = $(CROSS)-gcc
LD       = $(CROSS)-ld
KERNEL   = kernel.elf
ISO      = PinguinOS.iso
BUILD    = build
ISODIR   = ../iso

CFLAGS  = -m32 -std=c11 -ffreestanding -fno-stack-protector \
           -fno-pic -fno-builtin -nostdlib -Wall -Wextra \
           -Wno-unused-parameter -O2 -I./include

ASFLAGS = -m32 -c
LDFLAGS = -m elf_i386 -T linker.ld --no-undefined -z noexecstack

C_SRCS = kernel.c klib.c entropy.c \
    cpu/gdt.c cpu/idt.c cpu/cpu.c cpu/ssp.c \
    mm/phys_alloc.c mm/paging.c mm/heap.c mm/slab.c mm/process.c \
    drivers/vga.c drivers/serial.c drivers/pci.c drivers/keyboard.c \
    drivers/rtc.c drivers/acpi.c drivers/ide.c drivers/bga.c \
    drivers/mouse.c drivers/watchdog.c drivers/e1000.c \
    sched/scheduler.c \
    sync/sync.c \
    fs/vfs.c fs/ramfs.c fs/devfs.c fs/fat32.c fs/partition.c fs/procfs.c \
    net/net.c net/dhcp.c net/dns.c net/loopback.c net/tcp.c \
    ipc/syscall.c ipc/env.c ipc/pipe.c \
    debug/debug_shell.c

ASM_SRCS = boot.S cpu/isr.S

C_OBJS   = $(patsubst %.c, $(BUILD)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S, $(BUILD)/%.o, $(ASM_SRCS))
ALL_OBJS = $(ASM_OBJS) $(C_OBJS)

.PHONY: all iso run run-kernel debug run-disk clean size

all: $(BUILD)/$(KERNEL)

$(BUILD)/$(KERNEL): $(ALL_OBJS)
	@echo "  LD    $@"
	@$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(AS) $(ASFLAGS) $< -o $@

iso: $(BUILD)/$(KERNEL)
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(BUILD)/$(KERNEL) $(ISODIR)/boot/kernel.elf
	@grub-mkrescue -o $(ISO) $(ISODIR) 2>/dev/null || grub2-mkrescue -o $(ISO) $(ISODIR)

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 512M -serial stdio -boot d \
	    -no-reboot -no-shutdown

run-kernel: $(BUILD)/$(KERNEL)
	qemu-system-i386 -kernel $(BUILD)/$(KERNEL) -m 512M \
	    -serial stdio -no-reboot -no-shutdown

debug: $(BUILD)/$(KERNEL)
	qemu-system-i386 -kernel $(BUILD)/$(KERNEL) -m 512M \
	    -serial stdio -s -S -no-reboot

run-disk: iso
	@[ -f disk.img ] || dd if=/dev/zero of=disk.img bs=1M count=64
	qemu-system-i386 -cdrom $(ISO) -m 512M -serial stdio \
	    -drive file=disk.img,format=raw,if=ide -boot d -no-reboot

clean:
	@rm -rf $(BUILD) $(ISO)

size: $(BUILD)/$(KERNEL)
	@$(CROSS)-size $(BUILD)/$(KERNEL)
