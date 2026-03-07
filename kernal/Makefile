# ============================================================
#  PinguinOS Kernel  –  Makefile
#  Targets:  all   iso   run   clean   debug
# ============================================================

# ── Cross-Compiler (Präfix anpassen, falls nötig) ─────────────
CROSS   ?= i686-elf
CC       = $(CROSS)-gcc
AS       = $(CROSS)-gcc   # GCC zum Assemblieren von .S nutzen (wegen Präprozessor)
LD       = $(CROSS)-ld
OBJCOPY  = $(CROSS)-objcopy

KERNEL   = kernel.elf
ISO      = PinguinOS.iso

# ── Verzeichnisse ─────────────────────────────────────────────
BUILD    = build
ISODIR   = ../iso

# ── Compiler-Flags ────────────────────────────────────────────
#
#  -m32                    : 32-Bit i686 Ausgabe
#  -ffreestanding          : Keine Host-libc, kein crt0
#  -nostdlib               : Standard-Bibliotheken nicht verlinken
#  -fno-stack-protector    : Kein Stack-Canary (wir haben kein __stack_chk_fail)
#  -fno-pic                : Kein positionsunabhängiger Code
#  -mno-red-zone           : x86-64 ABI-Sache; harmlos auf i686
#  -Wall -Wextra           : Warnungen aktivieren
#  -O2                     : Optimieren (nutzen Sie -O0 für einfacheres Debugging)
#

CFLAGS  = -m32                  \
           -std=c11              \
           -ffreestanding        \
           -fno-stack-protector  \
           -fno-pic              \
           -fno-builtin          \
           -nostdlib             \
           -Wall                 \
           -Wextra               \
           -Wno-unused-parameter \
           -O2                   \
           -I./include

ASFLAGS = -m32 -c

LDFLAGS = -m elf_i386            \
           -T linker.ld           \
           --no-undefined         \
           -z noexecstack

# ── Quelldateien ──────────────────────────────────────────────
C_SRCS  = kernel.c               \
           klib.c                 \
           cpu/gdt.c              \
           cpu/idt.c              \
           cpu/cpu.c              \
           mm/phys_alloc.c        \
           mm/paging.c            \
           mm/heap.c              \
           drivers/vga.c          \
           drivers/serial.c       \
           drivers/pci.c          \
           sched/scheduler.c

ASM_SRCS = boot.S                 \
             cpu/isr.S

# ── Objektdateien ─────────────────────────────────────────────
C_OBJS   = $(patsubst %.c,   $(BUILD)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S,   $(BUILD)/%.o, $(ASM_SRCS))
ALL_OBJS = $(ASM_OBJS) $(C_OBJS)

# ── Standard-Target ───────────────────────────────────────────
.PHONY: all
all: $(BUILD)/$(KERNEL)

# ── Linker-Vorgang ────────────────────────────────────────────
$(BUILD)/$(KERNEL): $(ALL_OBJS)
	@echo "  LD    $@"
	@$(LD) $(LDFLAGS) -o $@ $^

# ── C-Quellen kompilieren ─────────────────────────────────────
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ── .S-Quellen assemblieren ───────────────────────────────────
$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(AS) $(ASFLAGS) $< -o $@

# ── ISO-Erstellung ────────────────────────────────────────────
.PHONY: iso
iso: $(BUILD)/$(KERNEL)
	@echo "  ISO   $(ISO)"
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(BUILD)/$(KERNEL) $(ISODIR)/boot/kernel.elf
	@grub-mkrescue -o $(ISO) $(ISODIR) 2>/dev/null || \
	 grub2-mkrescue -o $(ISO) $(ISODIR)

# ── In QEMU ausführen ─────────────────────────────────────────
#
# Flags:
#   -cdrom          : Von ISO booten
#   -m 512M         : 512 MB RAM
#   -serial stdio   : COM1 an das Host-Terminal weiterleiten (Debug-Log)
#   -boot d         : Von CD-ROM booten
#   -no-reboot      : Bei Triple-Fault anhalten statt neu zu starten
#   -no-shutdown    : QEMU nach dem Herunterfahren offen lassen
#
.PHONY: run
run: iso
	qemu-system-i386                  \
	    -cdrom $(ISO)                 \
	    -m 512M                       \
	    -serial stdio                 \
	    -boot d                       \
	    -no-reboot                    \
	    -no-shutdown

# ── Kernel direkt laden (für schnellere Iteration) ────────────
.PHONY: run-kernel
run-kernel: $(BUILD)/$(KERNEL)
	qemu-system-i386                  \
	    -kernel $(BUILD)/$(KERNEL)    \
	    -m 512M                       \
	    -serial stdio                 \
	    -no-reboot                    \
	    -no-shutdown

# ── Debug: QEMU + GDB stub ────────────────────────────────────
#
# Verwendung:
#   make debug     (startet QEMU und wartet auf GDB auf Port 1234)
#   in einem anderen Terminal:
#   i686-elf-gdb build/kernel.elf \
140: #       -ex "target remote :1234" \
141: #       -ex "layout src"
#
.PHONY: debug
debug: $(BUILD)/$(KERNEL)
	qemu-system-i386                  \
	    -kernel $(BUILD)/$(KERNEL)    \
	    -m 512M                       \
	    -serial stdio                 \
	    -s -S                         \
	    -no-reboot

# ── Bereinigen ────────────────────────────────────────────────
.PHONY: clean
clean:
	@echo "  CLEAN"
	@rm -rf $(BUILD) $(ISO)

# ── Größen anzeigen ───────────────────────────────────────────
.PHONY: size
size: $(BUILD)/$(KERNEL)
	@i686-elf-size $(BUILD)/$(KERNEL)
