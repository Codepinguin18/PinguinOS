#!/usr/bin/env python3
"""
build.py – PinguinOS Unified Build-System
==========================================
Baut Kernel + Desktop-OS in einem Schritt.

Verwendung (aus dem PinguinOS/ Wurzel-Verzeichnis):
  python3 build.py            → build + iso + run
  python3 build.py build      → nur kompilieren
  python3 build.py iso        → ISO erstellen (benötigt build)
  python3 build.py run        → QEMU starten   (benötigt iso)
  python3 build.py all        → build + iso + run
  python3 build.py clean      → alle Artefakte löschen
  python3 build.py info       → Projektübersicht

Voraussetzungen:
  sudo apt install gcc-i686-linux-gnu binutils-i686-linux-gnu \\
                   grub-pc-bin xorriso qemu-system-x86
  pip install Pillow
"""

import os, sys, subprocess, shutil, glob, datetime, time

# ══════════════════════════════════════════════════════════════
#  PFADE
# ══════════════════════════════════════════════════════════════
ROOT       = os.path.dirname(os.path.abspath(__file__))
KERNAL     = os.path.join(ROOT,   "kernal")
OS_DIR     = os.path.join(ROOT,   "os")
OS_SRC     = os.path.join(OS_DIR, "src")
OS_INC     = os.path.join(OS_DIR, "include")
OS_ICONS   = os.path.join(OS_DIR, "icons")
BUILD      = os.path.join(ROOT,   "build")      # einheitliches build/
ISO_DIR    = os.path.join(BUILD,  "iso")
KERNEL_ELF = os.path.join(BUILD,  "kernel.elf")
ISO_FILE   = os.path.join(BUILD,  "pinguinos.iso")
LINKER_LD  = os.path.join(KERNAL, "linker.ld")

CROSS  = "i686-linux-gnu"
CC     = f"{CROSS}-gcc"
AS     = f"{CROSS}-gcc"   # gcc als Assembler (verarbeitet .S)
LD     = f"{CROSS}-ld"
GRUB   = "grub-mkrescue"
QEMU   = "qemu-system-i386"

# ══════════════════════════════════════════════════════════════
#  COMPILER-FLAGS
# ══════════════════════════════════════════════════════════════
CFLAGS = [
    "-m32", "-std=c11", "-O2",
    "-Wall", "-Wextra", "-Wno-unused-parameter",
    "-ffreestanding", "-fno-stack-protector",
    "-fno-builtin", "-nostdlib", "-nostdinc",
    f"-I{KERNAL}/include",
    f"-I{OS_INC}",
]

ASFLAGS = [
    "-m32", "-ffreestanding", "-nostdlib",
    f"-I{KERNAL}/include",
]

LDFLAGS = [
    "-m", "elf_i386",
    "--oformat", "elf32-i386",
    "-T", LINKER_LD,
]

QEMU_FLAGS = [
    "-m",      "256M",
    "-vga",    "std",           # Standard-VGA + Bochs-VBE (BGA I/O-Ports 0x01CE/0x01CF aktiv)
    "-serial", "stdio",
    "-no-reboot",
    "-boot",   "d",             # Von CD-ROM booten (verhindert iPXE/Netzwerk-Boot)
    "-cdrom",  ISO_FILE,
    # USB-Tablet: absolute Maus-Koordinaten (kein Grab nötig)
    "-usb",
    "-device", "usb-tablet",
]

# ══════════════════════════════════════════════════════════════
#  TERMINAL-AUSGABE  (Pinguin-Browser-Stil)
# ══════════════════════════════════════════════════════════════
TEAL    = "\033[96m"
WHITE_T = "\033[97m"
GRAY_T  = "\033[90m"
GREEN   = "\033[92m"
RED_T   = "\033[91m"
YELLOW  = "\033[93m"
RESET   = "\033[0m"
BOLD    = "\033[1m"

def step(msg):   print(f"\n{TEAL}┌─{RESET} {BOLD}{msg}{RESET}")
def info(msg):   print(f"  {TEAL}••{RESET} {WHITE_T}{msg}{RESET}")
def ok(msg):     print(f"  {GREEN}✓{RESET}  {msg}")
def warn(msg):   print(f"  {YELLOW}!{RESET}  {YELLOW}{msg}{RESET}")
def fail(msg):   print(f"  {RED_T}✗{RESET}  {RED_T}{BOLD}{msg}{RESET}"); sys.exit(1)
def hdr(title):
    w = 62
    print(f"\n{TEAL}{'═'*w}{RESET}")
    print(f"{TEAL}  {BOLD}PinguinOS Build  –  {title}{RESET}")
    print(f"{TEAL}{'═'*w}{RESET}")

# ══════════════════════════════════════════════════════════════
#  ICON-HEADER  (Pillow)
# ══════════════════════════════════════════════════════════════

IC = 0x00FFF0; IW = 0xFFFFFF; IB = 0x07070F; IG = 0x555566; IR = 0xFF4444; __ = 0

FALLBACK_ICONS = {
    "folder":   [__,__,__,__,__,__,__,__,__,__,__,IC,IC,IC,__,__,__,__,__,__,IC,IC,IC,IC,IC,IC,IC,IC,IC,__,IC,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IC,IC,IC,IC,IC,IC,IC,IC,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__],
    "terminal": [IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,IC,__,__,__,__,__,__,IC,IB,IB,__,IC,IC,__,__,__,__,__,IB,IB,__,__,IC,IC,__,__,__,__,IB,IB,__,IC,IC,__,__,__,__,__,IB,IB,__,__,__,IC,IC,IC,__,__,IB,IB,__,__,__,__,__,__,__,__,IB,IB,IC,__,__,__,__,__,__,IC,IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,__,__,__,__,__,__,__,__,__,__],
    "settings": [__,__,__,IC,IC,IC,IC,__,__,__,__,__,IC,IG,IG,IG,IG,IC,__,__,__,IC,IG,IG,IW,IW,IG,IG,IC,__,IC,IG,IG,IW,IW,IW,IW,IG,IG,IC,IC,IG,IW,IW,IW,IW,IW,IW,IG,IC,IC,IG,IW,IW,IW,IW,IW,IW,IG,IC,IC,IG,IG,IW,IW,IW,IW,IG,IG,IC,__,IC,IG,IG,IW,IW,IG,IG,IC,__,__,__,IC,IG,IG,IG,IG,IC,__,__,__,__,__,IC,IC,IC,IC,__,__,__],
    "user":     [__,__,__,IC,IC,IC,IC,__,__,__,__,__,IC,IW,IW,IW,IW,IC,__,__,__,__,IC,IW,IW,IW,IW,IC,__,__,__,__,__,IC,IC,IC,IC,__,__,__,__,IC,IC,__,__,__,__,IC,IC,__,IC,IW,IW,IC,IC,IC,IC,IW,IW,IC,IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IC,IC,IC,IC,IC,IC,IC,__,__,__,__,__,__,__,__,__,__,__],
    "power":    [__,__,__,IR,IR,IR,IR,__,__,__,__,__,IR,__,__,__,__,IR,__,__,__,IR,__,__,IW,IW,__,__,IR,__,IR,__,__,IW,IW,IW,IW,__,__,IR,IR,__,__,IW,IW,IW,IW,__,__,IR,IR,__,__,IW,IW,IW,IW,__,__,IR,__,IR,__,__,IW,IW,__,__,IR,__,__,__,IR,IR,__,__,IR,IR,__,__,__,__,__,IR,IR,IR,IR,__,__,__,__,__,__,__,__,__,__,__,__,__],
    "penguin":  [__,__,IC,IC,IC,IC,IC,IC,__,__,__,IC,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IC,IC,IW,IW,IC,IC,IW,IC,IC,IW,IC,IW,IW,IW,IW,IC,IW,IC,IC,IW,IW,IC,IC,IC,IC,IW,IW,IC,IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IC,IW,IW,IC,IW,IC,__,__,IC,IC,IC,IW,IW,IC,IC,IC,__,__,__,IC,IC,IC,IC,IC,IC,__,__,__,__,__,__,__,__,__,__,__,__],
    "info":     [__,__,IC,IC,IC,IC,IC,IC,__,__,__,IC,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,IC,IW,IC,IW,IW,IC,IW,IW,IW,IC,IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,__,IC,IW,IW,IW,IW,IW,IW,IC,__,__,__,IC,IC,IC,IC,IC,IC,__,__],
    "browser":  [__,IC,IC,IC,IC,IC,IC,IC,__,__,__,IC,IW,IW,IW,IW,IW,IC,IC,__,__,IC,IW,IW,IW,IW,IW,IW,IC,__,__,IC,IW,IC,IC,IW,IW,IW,IC,__,__,IC,IW,IC,IC,IW,IW,IW,IC,__,__,IC,IW,IC,IC,IW,IW,IC,__,__,__,IC,IW,IW,IW,IW,IC,__,__,__,__,IC,IW,IW,IW,IW,IW,IC,__,__,__,IC,IC,IC,IC,IC,IC,IC,__,__,__,__,__,__,__,__,__,__,__,__],
    "file":     [__,IC,IC,IC,IC,IC,IC,__,__,__,__,IC,IW,IW,IW,IC,IC,IC,__,__,__,IC,IW,IW,IC,IW,IW,IC,__,__,__,IC,IW,IW,IW,IW,IW,IC,__,__,__,IC,IG,IG,IG,IG,IG,IC,__,__,__,IC,IW,IW,IW,IW,IW,IC,__,__,__,IC,IG,IG,IG,IG,IG,IC,__,__,__,IC,IW,IW,IW,IW,IW,IC,__,__,__,IC,IC,IC,IC,IC,IC,IC,__,__,__,__,__,__,__,__,__,__,__,__],
}

def _generate_icons_h():
    """Generiert os/include/icons.h (aus PNGs oder Fallback-Arrays)."""
    os.makedirs(OS_INC,   exist_ok=True)
    os.makedirs(OS_ICONS, exist_ok=True)

    png_icons = {}
    try:
        from PIL import Image
        for fname in sorted(os.listdir(OS_ICONS)):
            if not fname.lower().endswith(".png"):
                continue
            name = os.path.splitext(fname)[0].lower()
            img  = Image.open(os.path.join(OS_ICONS, fname)).convert("RGBA")
            img  = img.resize((10, 10), Image.LANCZOS)
            pix  = []
            for r2 in range(10):
                for c2 in range(10):
                    rv, gv, bv, av = img.getpixel((c2, r2))
                    pix.append(0 if av < 128 else (rv << 16) | (gv << 8) | bv)
            png_icons[name] = pix
            ok(f"PNG: {fname} → ICON_{name.upper()}")
    except ImportError:
        warn("Pillow nicht installiert – Fallback-Icons aktiv  (pip install Pillow)")

    all_icons = {**FALLBACK_ICONS, **png_icons}

    lines = [
        "/* icons.h – auto-generiert von build.py */",
        f"/* {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')} */",
        "#ifndef _OS_ICONS_H", "#define _OS_ICONS_H",
        '#include "../../kernal/include/types.h"',
        "#define __BG__ 0x00000000", "",
    ]
    for name, pixels in all_icons.items():
        assert len(pixels) == 100, f"Icon '{name}' hat {len(pixels)} Pixel!"
        lines.append(f"/* {name} */")
        lines.append(f"static const uint32_t ICON_{name.upper()}[100] = {{")
        for row in range(10):
            vals = ["__BG__" if pixels[row*10+c] == 0
                    else f"0x{pixels[row*10+c]:06X}" for c in range(10)]
            lines.append("    " + ", ".join(vals) + ",")
        lines.append("};")
        lines.append("")
    lines += ["#undef __BG__", "#endif /* _OS_ICONS_H */", ""]

    out = os.path.join(OS_INC, "icons.h")
    with open(out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    ok(f"icons.h  ({len(all_icons)} Icons)")

# ══════════════════════════════════════════════════════════════
#  COMPILE HELPERS
# ══════════════════════════════════════════════════════════════

def _run(cmd, cwd=None):
    """Führt Befehl aus, bricht bei Fehler ab."""
    r = subprocess.run(cmd, cwd=cwd)
    if r.returncode != 0:
        fail(f"Befehl fehlgeschlagen: {' '.join(cmd)}")

def _compile_c(src, obj):
    """Kompiliert eine .c-Datei zu .o."""
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    # Nur neu kompilieren wenn Quelle neuer als Objekt
    if os.path.exists(obj) and os.path.getmtime(src) <= os.path.getmtime(obj):
        return False   # kein Rebuild nötig
    print(f"  {TEAL}CC{RESET}    {os.path.relpath(src, ROOT)}")
    _run([CC] + CFLAGS + ["-c", src, "-o", obj])
    return True

def _compile_s(src, obj):
    """Assembliert eine .S-Datei zu .o."""
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    if os.path.exists(obj) and os.path.getmtime(src) <= os.path.getmtime(obj):
        return False
    print(f"  {TEAL}AS{RESET}    {os.path.relpath(src, ROOT)}")
    _run([AS] + ASFLAGS + ["-c", src, "-o", obj])
    return True

# ══════════════════════════════════════════════════════════════
#  QUELLDATEI-LISTE
# ══════════════════════════════════════════════════════════════

def _kernel_sources():
    """
    Gibt alle (src, obj) Paare des Kernels zurück.
    Reihenfolge: boot.S zuerst, dann alle .c-Dateien.
    """
    K = KERNAL
    B = os.path.join(BUILD, "kernal")

    # Assembler-Quellen (müssen zuerst gelinkt werden)
    asm_srcs = [
        (os.path.join(K, "boot.S"),         os.path.join(B, "boot.o")),
        (os.path.join(K, "cpu", "isr.S"),   os.path.join(B, "cpu", "isr.o")),
    ]

    # C-Quellen aus build.log rekonstruiert + bekannte Session-Dateien
    c_rel = [
        "kernel.c", "klib.c", "entropy.c",
        # cpu/
        "cpu/gdt.c", "cpu/idt.c", "cpu/cpu.c", "cpu/ssp.c",
        "cpu/cpu_topo.c",
        # mm/
        "mm/phys_alloc.c", "mm/paging.c", "mm/heap.c",
        "mm/slab.c", "mm/process.c", "mm/shmem.c",
        "mm/swap.c",
        # drivers/
        "drivers/vga.c", "drivers/serial.c", "drivers/pci.c",
        "drivers/keyboard.c", "drivers/rtc.c", "drivers/acpi.c",
        "drivers/ide.c", "drivers/bga.c", "drivers/mouse.c",
        "drivers/vmmouse.c",
        "drivers/watchdog.c", "drivers/e1000.c",
        "drivers/apic.c", "drivers/rtl8139.c",
        "drivers/ahci.c",
        # sched/
        "sched/scheduler.c",
        # sync/
        "sync/sync.c",
        # fs/
        "fs/vfs.c", "fs/ramfs.c", "fs/devfs.c", "fs/fat32.c",
        "fs/partition.c", "fs/procfs.c", "fs/pagecache.c",
        "fs/iso9660.c", "fs/sysfs.c", "fs/initrd.c",
        "fs/ext2.c",
        # net/
        "net/net.c", "net/dhcp.c", "net/dns.c",
        "net/loopback.c", "net/tcp.c",
        # ipc/
        "ipc/syscall.c", "ipc/env.c", "ipc/pipe.c",
        "ipc/msgqueue.c", "ipc/unix_sock.c",
        # debug/
        "debug/debug_shell.c", "debug/kprobes.c",
        "debug/gdbstub.c", "debug/crashdump.c",
        "debug/profiler.c",
        # PE/EXE-Loader
        "fs/pe.c",
    ]

    c_srcs = []
    for rel in c_rel:
        src = os.path.join(K, rel)
        if os.path.exists(src):
            obj = os.path.join(B, rel.replace(".c", ".o"))
            c_srcs.append((src, obj))
        else:
            warn(f"Quelle nicht gefunden (übersprungen): kernal/{rel}")

    return asm_srcs, c_srcs


def _os_sources():
    """Gibt alle (src, obj) Paare des OS zurück."""
    B = os.path.join(BUILD, "os")
    srcs = ["ui.c", "login.c", "desktop.c", "pinguinos.c", "app.c"]
    result = []
    for s in srcs:
        src = os.path.join(OS_SRC, s)
        if os.path.exists(src):
            result.append((src, os.path.join(B, s.replace(".c", ".o"))))
        else:
            warn(f"OS-Quelle nicht gefunden: os/src/{s}")
    return result

# ══════════════════════════════════════════════════════════════
#  BUILD
# ══════════════════════════════════════════════════════════════

def cmd_build():
    hdr("Build")
    t0 = time.time()

    os.makedirs(BUILD, exist_ok=True)

    # 1) Icons generieren
    step("Icons generieren")
    _generate_icons_h()

    # 2) Kernel-Quellen kompilieren
    step("Kernel kompilieren")
    asm_srcs, c_srcs = _kernel_sources()
    rebuilt = 0
    for src, obj in asm_srcs:
        if _compile_s(src, obj):
            rebuilt += 1
    for src, obj in c_srcs:
        if _compile_c(src, obj):
            rebuilt += 1
    if rebuilt == 0:
        info("Keine Kernel-Änderungen – übersprungen")

    # 3) OS-Quellen kompilieren
    step("OS kompilieren")
    os_srcs = _os_sources()
    rebuilt = 0
    for src, obj in os_srcs:
        if _compile_c(src, obj):
            rebuilt += 1
    if rebuilt == 0:
        info("Keine OS-Änderungen – übersprungen")

    # 4) Linken
    step("Linken → kernel.elf")
    all_objs = (
        [obj for _, obj in asm_srcs] +
        [obj for _, obj in c_srcs]   +
        [obj for _, obj in os_srcs]
    )
    # Nur vorhandene .o-Dateien übergeben
    existing_objs = [o for o in all_objs if os.path.exists(o)]
    missing = [o for o in all_objs if not os.path.exists(o)]
    if missing:
        warn(f"{len(missing)} Objekte fehlen (wurden übersprungen):")
        for m in missing[:5]:
            warn(f"  {os.path.relpath(m, ROOT)}")

    print(f"  {TEAL}LD{RESET}    build/kernel.elf  ({len(existing_objs)} Objekte)")
    _run([LD] + LDFLAGS + existing_objs + ["-o", KERNEL_ELF])

    elapsed = time.time() - t0
    print(f"\n  {GREEN}{BOLD}✓  Build erfolgreich  ({elapsed:.1f}s){RESET}\n")


# ══════════════════════════════════════════════════════════════
#  ISO
# ══════════════════════════════════════════════════════════════

def cmd_iso():
    hdr("ISO erstellen")
    if not os.path.exists(KERNEL_ELF):
        fail("kernel.elf nicht gefunden – bitte zuerst: python3 build.py build")

    # ISO-Verzeichnisstruktur aufbauen
    boot_dir = os.path.join(ISO_DIR, "boot", "grub")
    os.makedirs(boot_dir, exist_ok=True)

    # kernel.elf kopieren
    shutil.copy2(KERNEL_ELF, os.path.join(ISO_DIR, "boot", "kernel.elf"))
    ok("kernel.elf → iso/boot/kernel.elf")

    # grub.cfg immer neu schreiben (timeout=0, kein Menü)
    with open(os.path.join(boot_dir, "grub.cfg"), "w") as f:
        f.write('set timeout=0\nset timeout_style=hidden\nset default=0\n'
                'menuentry "PinguinOS" {\n'
                '    multiboot /boot/kernel.elf\n'
                '    boot\n}\n')
        ok("grub.cfg (minimal) erstellt")

    # ISO bauen
    print(f"  {TEAL}ISO{RESET}   {ISO_FILE}")
    _run([GRUB, "-o", ISO_FILE, ISO_DIR])
    size = os.path.getsize(ISO_FILE) // 1024
    print(f"\n  {GREEN}{BOLD}✓  ISO erstellt: {ISO_FILE}  ({size} KB){RESET}\n")


# ══════════════════════════════════════════════════════════════
#  RUN
# ══════════════════════════════════════════════════════════════

def cmd_run():
    hdr("QEMU starten")
    if not os.path.exists(ISO_FILE):
        fail("ISO nicht gefunden – bitte zuerst: python3 build.py iso")

    print(f"  {TEAL}QEMU{RESET}  {' '.join(QEMU_FLAGS)}")
    print(f"  {GRAY_T}(Fenster schließen oder Strg+C zum Beenden){RESET}\n")
    subprocess.run([QEMU] + QEMU_FLAGS)


# ══════════════════════════════════════════════════════════════
#  CLEAN
# ══════════════════════════════════════════════════════════════

def cmd_clean():
    hdr("Clean")
    for path in [BUILD, os.path.join(OS_INC, "icons.h")]:
        if os.path.isdir(path):
            shutil.rmtree(path)
            ok(f"Gelöscht: {os.path.relpath(path, ROOT)}/")
        elif os.path.isfile(path):
            os.remove(path)
            ok(f"Gelöscht: {os.path.relpath(path, ROOT)}")
    print()


# ══════════════════════════════════════════════════════════════
#  INFO
# ══════════════════════════════════════════════════════════════

def cmd_info():
    hdr("Projektübersicht")
    print(f"\n  {TEAL}Verzeichnisse:{RESET}")
    for label, path in [("Wurzel", ROOT), ("Kernel", KERNAL),
                         ("OS", OS_DIR), ("Build", BUILD)]:
        exists = f"{GREEN}✓{RESET}" if os.path.isdir(path) else f"{RED_T}✗{RESET}"
        print(f"    {exists} {label:<10} {path}")

    print(f"\n  {TEAL}Tools:{RESET}")
    for tool in [CC, AS, LD, GRUB, QEMU]:
        found = shutil.which(tool)
        status = f"{GREEN}✓  {found}{RESET}" if found else f"{RED_T}✗  nicht gefunden{RESET}"
        print(f"    {tool:<30} {status}")

    print(f"\n  {TEAL}Artefakte:{RESET}")
    for label, path in [("kernel.elf", KERNEL_ELF), ("pinguinos.iso", ISO_FILE)]:
        if os.path.exists(path):
            sz = os.path.getsize(path) // 1024
            ok(f"{label:<20} {sz} KB")
        else:
            print(f"  {GRAY_T}–{RESET}  {label:<20} nicht vorhanden")

    asm_srcs, c_srcs = _kernel_sources()
    os_srcs = _os_sources()
    print(f"\n  {TEAL}Quellen:{RESET}")
    print(f"    Kernel .S : {len(asm_srcs)}")
    print(f"    Kernel .c : {len([s for s,_ in c_srcs if os.path.exists(s)])}")
    print(f"    OS     .c : {len([s for s,_ in os_srcs if os.path.exists(s)])}")

    pngs = [f for f in os.listdir(OS_ICONS)
            if f.lower().endswith(".png")] if os.path.isdir(OS_ICONS) else []
    print(f"\n  {TEAL}Icons in os/icons/ ({len(pngs)} PNG):{RESET}")
    if pngs:
        for f in sorted(pngs):
            print(f"    {f}")
    else:
        warn("Keine PNG-Dateien – Fallback-Icons aktiv")
        print(f"    PNG ablegen in: {OS_ICONS}")
    print()


# ══════════════════════════════════════════════════════════════
#  EINSTIEGSPUNKT
# ══════════════════════════════════════════════════════════════

COMMANDS = {
    "build": cmd_build,
    "iso":   cmd_iso,
    "run":   cmd_run,
    "clean": cmd_clean,
    "info":  cmd_info,
    "all":   lambda: (cmd_build(), cmd_iso(), cmd_run()),
}

if __name__ == "__main__":
    cmd = sys.argv[1].lower() if len(sys.argv) > 1 else "all"
    if cmd not in COMMANDS:
        print(f"Verwendung: python3 build.py [{' | '.join(COMMANDS)}]")
        sys.exit(1)
    COMMANDS[cmd]()