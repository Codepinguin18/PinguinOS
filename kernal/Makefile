# ============================================================
#  PinguinOS 2.3 Kernel  –  Makefile v2.3
#  Ziele:  all   iso   run   run-kernel   debug   clean   size
# ============================================================

CROSS   ?= i686-elf
CC       = $(CROSS)-gcc
AS       = $(CROSS)-gcc
LD       = $(CROSS)-ld
KERNEL   = kernel.elf
ISO      = PinguinOS.iso
BUILD    = build
ISODIR   = ../iso

# ── Compiler-Flags ───────────────────────────────────────────
CFLAGS  = -m32 -std=c11 -ffreestanding -fstack-protector-strong \
           -fno-pic -fno-builtin -nostdlib -Wall -Wextra \
           -Wno-unused-parameter -O2 -I./include

# ── Assembler-Flags ──────────────────────────────────────────
ASFLAGS = -m32 -c

# ── Linker-Flags ─────────────────────────────────────────────
LDFLAGS = -m elf_i386 -T linker.ld --no-undefined -z noexecstack

# ── C-Quelldateien ───────────────────────────────────────────
C_SRCS = \
    kernel.c klib.c entropy.c \
    \
    cpu/gdt.c cpu/idt.c cpu/cpu.c cpu/ssp.c \
    cpu/cpu_topo.c \
    cpu/smp.c \
    \
    mm/phys_alloc.c mm/paging.c mm/heap.c mm/slab.c \
    mm/process.c mm/shmem.c mm/swap.c \
    \
    drivers/vga.c drivers/serial.c drivers/pci.c \
    drivers/keyboard.c drivers/rtc.c drivers/acpi.c \
    drivers/ide.c drivers/ahci.c \
    drivers/bga.c drivers/mouse.c \
    drivers/watchdog.c drivers/e1000.c drivers/rtl8139.c \
    drivers/apic.c drivers/ioapic.c \
    drivers/usb.c \
    drivers/nvme.c \
    drivers/ac97.c \
    \
    sched/scheduler.c \
    sync/sync.c \
    \
    fs/vfs.c fs/ramfs.c fs/devfs.c fs/fat32.c \
    fs/partition.c fs/procfs.c fs/pagecache.c \
    fs/iso9660.c fs/sysfs.c fs/initrd.c fs/ext2.c \
    \
    net/net.c net/dhcp.c net/dns.c net/loopback.c net/tcp.c \
    \
    ipc/syscall.c ipc/env.c ipc/pipe.c ipc/msgqueue.c \
    ipc/unix_sock.c \
    \
    debug/debug_shell.c debug/kprobes.c debug/gdbstub.c \
    debug/profiler.c debug/crashdump.c

# ── Assembler-Quelldateien ────────────────────────────────────
ASM_SRCS = boot.S cpu/isr.S

# ── Objekt-Dateien ────────────────────────────────────────────
C_OBJS   = $(patsubst %.c, $(BUILD)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S, $(BUILD)/%.o, $(ASM_SRCS))
ALL_OBJS = $(ASM_OBJS) $(C_OBJS)

.PHONY: all iso run run-kernel debug run-disk clean size

# ── Standard-Ziel ────────────────────────────────────────────
all: $(BUILD)/$(KERNEL)

$(BUILD)/$(KERNEL): $(ALL_OBJS)
	@echo "  LD    $@"
	@$(LD) $(LDFLAGS) -o $@ $^

# ── C-Kompilierung ────────────────────────────────────────────
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ── Assembly-Kompilierung ─────────────────────────────────────
$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(AS) $(ASFLAGS) $< -o $@

# ── ISO erstellen ─────────────────────────────────────────────
iso: $(BUILD)/$(KERNEL)
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(BUILD)/$(KERNEL) $(ISODIR)/boot/kernel.elf
	@grub-mkrescue -o $(ISO) $(ISODIR) 2>/dev/null || \
	    grub2-mkrescue -o $(ISO) $(ISODIR)

# ── QEMU-Ziele ────────────────────────────────────────────────
run: iso
	qemu-system-i386 -cdrom $(ISO) -m 512M -serial stdio \
	    -boot d -no-reboot -no-shutdown

run-kernel: $(BUILD)/$(KERNEL)
	qemu-system-i386 -kernel $(BUILD)/$(KERNEL) -m 512M \
	    -serial stdio -no-reboot -no-shutdown

debug: $(BUILD)/$(KERNEL)
	qemu-system-i386 -kernel $(BUILD)/$(KERNEL) -m 512M \
	    -serial stdio -s -S -no-reboot

# ── Disk-Image für FAT32-Tests ────────────────────────────────
run-disk: iso
	@[ -f disk.img ] || dd if=/dev/zero of=disk.img bs=1M count=64
	qemu-system-i386 -cdrom $(ISO) -m 512M -serial stdio \
	    -drive file=disk.img,format=raw,if=ide -boot d -no-reboot

# ── Aufräumen ─────────────────────────────────────────────────
clean:
	@rm -rf $(BUILD) $(ISO)

# ── Größenanzeige ─────────────────────────────────────────────
size: $(BUILD)/$(KERNEL)
	@$(CROSS)-size $(BUILD)/$(KERNEL)