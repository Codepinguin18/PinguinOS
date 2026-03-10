# ================================================================
# PinguinOS Desktop-Umgebung – Makefile
# ================================================================

CROSS   ?= i686-linux-gnu
CC       = $(CROSS)-gcc
OBJDIR   = build

CFLAGS   = -m32 -std=c11 -O2 -Wall -Wextra \
           -ffreestanding -fno-stack-protector \
           -fno-builtin -nostdlib \
           -I../kernal/include \
           -I./include

SRCS     = src/ui.c \
           src/login.c \
           src/desktop.c \
           src/pinguinos.c

OBJS     = $(SRCS:src/%.c=$(OBJDIR)/%.o)

.PHONY: all clean

all: $(OBJDIR) $(OBJS)
	@echo "  OS    Objekte fertig"

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.c
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)
	@echo "  CLEAN os/build"