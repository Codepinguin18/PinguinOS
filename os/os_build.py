#!/usr/bin/env python3
"""
os_build.py – PinguinOS Desktop-Umgebung Build-System
======================================================

Verwendung:
  python3 os_build.py build    → Icons laden + OS kompilieren
  python3 os_build.py icons    → Nur include/icons.h neu generieren
  python3 os_build.py clean    → Build-Artefakte löschen
  python3 os_build.py info     → Projektübersicht anzeigen

Icon-Workflow:
  1. PNG-Dateien in os/icons/ ablegen (beliebige Größe, RGBA/RGB)
  2. os_build.py liest sie, skaliert auf 10×10 Pixel (LANCZOS)
  3. Generiert include/icons.h mit C-uint32_t-Arrays
  4. Die C-Quellen benutzen ICON_<DATEINAME_GROSS> (z.B. ICON_FOLDER)

Ordner-Struktur (ALLE .h-Dateien in include/):
  os/
  ├── os_build.py
  ├── Makefile
  ├── icons/           ←  eigene PNG-Dateien hier ablegen
  ├── include/
  │   ├── ui.h
  │   ├── login.h
  │   ├── desktop.h
  │   ├── pinguinos.h
  │   ├── font8x8.h
  │   └── icons.h      ←  auto-generiert
  └── src/
      ├── ui.c
      ├── login.c
      ├── desktop.c
      └── pinguinos.c
"""

import os
import sys
import struct
import subprocess
import shutil
import datetime

try:
    from PIL import Image
    PIL_OK = True
except ImportError:
    PIL_OK = False

# ============================================================
# Pfade
# ============================================================
SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
ICONS_DIR   = os.path.join(SCRIPT_DIR, "icons")
INCLUDE_DIR = os.path.join(SCRIPT_DIR, "include")
SRC_DIR     = os.path.join(SCRIPT_DIR, "src")
BUILD_DIR   = os.path.join(SCRIPT_DIR, "build")
KERNEL_DIR  = os.path.join(SCRIPT_DIR, "..", "kernal")
KERNEL_OBJ  = os.path.join(KERNEL_DIR, "build", "os")
CROSS       = "i686-linux-gnu"
ICON_OUT    = os.path.join(INCLUDE_DIR, "icons.h")
ICON_SIZE   = 10   # Zielgröße

# ============================================================
# Terminal-Ausgabe  (Pinguin-Browser-Style)
# ============================================================
TEAL    = "\033[96m"
WHITE_T = "\033[97m"
GRAY_T  = "\033[90m"
GREEN   = "\033[92m"
RED_T   = "\033[91m"
YELLOW  = "\033[93m"
RESET   = "\033[0m"
BOLD    = "\033[1m"

def info(msg):  print(f"  {TEAL}••{RESET} {WHITE_T}{msg}{RESET}")
def ok(msg):    print(f"  {GREEN}✓{RESET}  {msg}")
def warn(msg):  print(f"  {YELLOW}!{RESET}  {YELLOW}{msg}{RESET}")
def err(msg):   print(f"  {RED_T}✗{RESET}  {RED_T}{msg}{RESET}")

def header(title):
    w = 60
    print()
    print(f"{TEAL}{'═' * w}{RESET}")
    print(f"{TEAL}  {BOLD}PinguinOS OS-Builder  –  {title}{RESET}")
    print(f"{TEAL}{'═' * w}{RESET}")


# ============================================================
# PNG → Liste von 100 uint32_t (0x00RRGGBB oder 0 = transparent)
# ============================================================

def png_to_pixels(path):
    """
    Lädt eine PNG (beliebige Größe), skaliert auf ICON_SIZE×ICON_SIZE,
    gibt genau 100 uint32_t-Werte zurück.
    Pixel mit alpha < 128 → 0 (transparent/Hintergrund).
    """
    if not PIL_OK:
        err("Pillow nicht installiert.  pip install Pillow")
        sys.exit(1)
    img    = Image.open(path).convert("RGBA")
    img    = img.resize((ICON_SIZE, ICON_SIZE), Image.LANCZOS)
    pixels = []
    for row in range(ICON_SIZE):
        for col in range(ICON_SIZE):
            r, g, b, a = img.getpixel((col, row))
            pixels.append(0 if a < 128 else (r << 16) | (g << 8) | b)
    assert len(pixels) == 100
    return pixels


# ============================================================
# Eingebettete Fallback-Icons (je genau 100 int-Einträge)
# ============================================================

IC = 0x00FFF0   # Teal
IW = 0xFFFFFF   # Weiß
IB = 0x07070F   # Dunkel
IG = 0x555566   # Grau
IR = 0xFF4444   # Rot
__ = 0          # Transparent

FALLBACK_ICONS = {
    "folder": [
        __,__,__,__,__,__,__,__,__,__,
        __,IC,IC,IC,__,__,__,__,__,__,
        IC,IC,IC,IC,IC,IC,IC,IC,IC,__,
        IC,IW,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IW,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IW,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IW,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IC,IC,IC,IC,IC,IC,IC,IC,__,
        __,__,__,__,__,__,__,__,__,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "terminal": [
        IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,
        IB,IC,__,__,__,__,__,__,IC,IB,
        IB,__,IC,IC,__,__,__,__,__,IB,
        IB,__,__,IC,IC,__,__,__,__,IB,
        IB,__,IC,IC,__,__,__,__,__,IB,
        IB,__,__,__,IC,IC,IC,__,__,IB,
        IB,__,__,__,__,__,__,__,__,IB,
        IB,IC,__,__,__,__,__,__,IC,IB,
        IB,IB,IB,IB,IB,IB,IB,IB,IB,IB,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "settings": [
        __,__,__,IC,IC,IC,IC,__,__,__,
        __,__,IC,IG,IG,IG,IG,IC,__,__,
        __,IC,IG,IG,IW,IW,IG,IG,IC,__,
        IC,IG,IG,IW,IW,IW,IW,IG,IG,IC,
        IC,IG,IW,IW,IW,IW,IW,IW,IG,IC,
        IC,IG,IW,IW,IW,IW,IW,IW,IG,IC,
        IC,IG,IG,IW,IW,IW,IW,IG,IG,IC,
        __,IC,IG,IG,IW,IW,IG,IG,IC,__,
        __,__,IC,IG,IG,IG,IG,IC,__,__,
        __,__,__,IC,IC,IC,IC,__,__,__,
    ],
    "user": [
        __,__,__,IC,IC,IC,IC,__,__,__,
        __,__,IC,IW,IW,IW,IW,IC,__,__,
        __,__,IC,IW,IW,IW,IW,IC,__,__,
        __,__,__,IC,IC,IC,IC,__,__,__,
        __,IC,IC,__,__,__,__,IC,IC,__,
        IC,IW,IW,IC,IC,IC,IC,IW,IW,IC,
        IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,
        IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,
        __,IC,IC,IC,IC,IC,IC,IC,IC,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "power": [
        __,__,__,IR,IR,IR,IR,__,__,__,
        __,__,IR,__,__,__,__,IR,__,__,
        __,IR,__,__,IW,IW,__,__,IR,__,
        IR,__,__,IW,IW,IW,IW,__,__,IR,
        IR,__,__,IW,IW,IW,IW,__,__,IR,
        IR,__,__,IW,IW,IW,IW,__,__,IR,
        __,IR,__,__,IW,IW,__,__,IR,__,
        __,__,IR,IR,__,__,IR,IR,__,__,
        __,__,__,IR,IR,IR,IR,__,__,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "penguin": [
        __,__,IC,IC,IC,IC,IC,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IW,IC,IC,IW,IW,IC,IC,IW,IC,
        IC,IW,IC,IW,IW,IW,IW,IC,IW,IC,
        IC,IW,IW,IC,IC,IC,IC,IW,IW,IC,
        IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,
        __,IC,IW,IC,IW,IW,IC,IW,IC,__,
        __,IC,IC,IC,IW,IW,IC,IC,IC,__,
        __,__,IC,IC,IC,IC,IC,IC,__,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "info": [
        __,__,IC,IC,IC,IC,IC,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IW,IC,__,
        IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,
        IC,IW,IC,IW,IW,IC,IW,IW,IW,IC,
        IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,
        IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,
        IC,IW,IW,IC,IC,IW,IW,IW,IW,IC,
        IC,IW,IW,IW,IW,IW,IW,IW,IW,IC,
        __,IC,IW,IW,IW,IW,IW,IW,IC,__,
        __,__,IC,IC,IC,IC,IC,IC,__,__,
    ],
    "browser": [
        __,IC,IC,IC,IC,IC,IC,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IC,IC,__,
        __,IC,IW,IW,IW,IW,IW,IW,IC,__,
        __,IC,IW,IC,IC,IW,IW,IW,IC,__,
        __,IC,IW,IC,IC,IW,IW,IW,IC,__,
        __,IC,IW,IC,IC,IW,IW,IC,__,__,
        __,IC,IW,IW,IW,IW,IC,__,__,__,
        __,IC,IW,IW,IW,IW,IW,IC,__,__,
        __,IC,IC,IC,IC,IC,IC,IC,__,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
    "file": [
        __,IC,IC,IC,IC,IC,IC,__,__,__,
        __,IC,IW,IW,IW,IC,IC,IC,__,__,
        __,IC,IW,IW,IC,IW,IW,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IC,__,__,
        __,IC,IG,IG,IG,IG,IG,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IC,__,__,
        __,IC,IG,IG,IG,IG,IG,IC,__,__,
        __,IC,IW,IW,IW,IW,IW,IC,__,__,
        __,IC,IC,IC,IC,IC,IC,IC,__,__,
        __,__,__,__,__,__,__,__,__,__,
    ],
}

# Validate all fallback icons at module load time
for _name, _pix in FALLBACK_ICONS.items():
    assert len(_pix) == 100, f"Fallback icon '{_name}' hat {len(_pix)} Pixel!"


# ============================================================
# Pixel-Liste → C-Array-String
# ============================================================

def pixels_to_c_array(name, pixels):
    assert len(pixels) == 100, f"'{name}': {len(pixels)} Pixel (erwartet 100)"
    c_name = f"ICON_{name.upper()}"
    lines  = [f"/* {name} */",
              f"static const uint32_t {c_name}[100] = {{"]
    for row in range(ICON_SIZE):
        vals = []
        for col in range(ICON_SIZE):
            v = pixels[row * ICON_SIZE + col]
            vals.append("__BG__" if v == 0 else f"0x{v:06X}")
        lines.append("    " + ", ".join(vals) + ",")
    lines.append("};")
    return "\n".join(lines)


# ============================================================
# include/icons.h generieren
# ============================================================

def generate_icon_header():
    os.makedirs(INCLUDE_DIR, exist_ok=True)
    os.makedirs(ICONS_DIR,   exist_ok=True)

    # PNG-Dateien aus icons/ laden (Vorrang vor Fallback)
    png_icons = {}
    if os.path.isdir(ICONS_DIR):
        for fname in sorted(os.listdir(ICONS_DIR)):
            if not fname.lower().endswith(".png"):
                continue
            name = os.path.splitext(fname)[0].lower()
            path = os.path.join(ICONS_DIR, fname)
            try:
                pixels = png_to_pixels(path)
                png_icons[name] = pixels
                ok(f"PNG: {fname}  →  ICON_{name.upper()}")
            except Exception as e:
                warn(f"Fehler beim Laden '{fname}': {e}  (Fallback aktiv)")

    if not png_icons:
        info("Keine PNGs in icons/ – eingebettete Fallback-Icons werden genutzt")
        info(f"PNG ablegen in: {ICONS_DIR}")

    # Zusammenführen: Fallback + PNG (PNG überschreibt Fallback)
    all_icons = {**FALLBACK_ICONS, **png_icons}

    lines = [
        "/**",
        " * @file icons.h",
        " * @brief PinguinOS 10×10 Icons – auto-generiert von os_build.py.",
        " *",
        " * Eigene Icons: PNG-Dateien in os/icons/ ablegen.",
        " * Namensschema: mein_icon.png  →  ICON_MEIN_ICON",
        " * PNG wird auf 10×10 skaliert (LANCZOS) und als C-Array eingebettet.",
        " *",
        f" * Generiert: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
        " */",
        "#ifndef _OS_ICONS_H",
        "#define _OS_ICONS_H",
        "",
        '#include "../../kernal/include/types.h"',
        "",
        "#define __BG__ 0x00000000   /* Transparenz / Hintergrund */",
        "",
    ]

    for name, pixels in all_icons.items():
        lines.append(pixels_to_c_array(name, pixels))
        lines.append("")

    lines += ["#undef __BG__", "", "#endif /* _OS_ICONS_H */", ""]

    with open(ICON_OUT, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    ok(f"include/icons.h  ({len(all_icons)} Icons gesamt)")


# ============================================================
# Kompilieren
# ============================================================

def build_os():
    info("Kompiliere OS-Quellen...")
    result = subprocess.run(["make", f"CROSS={CROSS}", "all"], cwd=SCRIPT_DIR)
    if result.returncode != 0:
        err("Kompilierung fehlgeschlagen!")
        return False

    os.makedirs(KERNEL_OBJ, exist_ok=True)
    copied = 0
    if os.path.isdir(BUILD_DIR):
        for fname in os.listdir(BUILD_DIR):
            if fname.endswith(".o"):
                shutil.copy2(os.path.join(BUILD_DIR, fname),
                             os.path.join(KERNEL_OBJ, fname))
                copied += 1
    if copied:
        ok(f"{copied} .o-Datei(en) → ../kernal/build/os/")
    return True


# ============================================================
# Clean
# ============================================================

def clean():
    for path in [BUILD_DIR, KERNEL_OBJ]:
        if os.path.exists(path):
            shutil.rmtree(path)
            ok(f"Gelöscht: {os.path.relpath(path, SCRIPT_DIR)}")
    if os.path.exists(ICON_OUT):
        os.remove(ICON_OUT)
        ok("Gelöscht: include/icons.h")


# ============================================================
# Info
# ============================================================

def show_info():
    header("Projektübersicht")
    print(f"\n  {TEAL}Pfade:{RESET}")
    print(f"    OS-Wurzel   {SCRIPT_DIR}")
    print(f"    Kernel      {KERNEL_DIR}")
    print(f"    Icons       {ICONS_DIR}")
    print(f"    Cross-CC    {CROSS}-gcc")

    print(f"\n  {TEAL}Quell-Dateien (src/):{RESET}")
    if os.path.isdir(SRC_DIR):
        for f in sorted(os.listdir(SRC_DIR)):
            if f.endswith(".c"):
                print(f"    src/{f}")

    print(f"\n  {TEAL}Header-Dateien (include/):{RESET}")
    if os.path.isdir(INCLUDE_DIR):
        for f in sorted(os.listdir(INCLUDE_DIR)):
            if f.endswith(".h"):
                gen = " (auto-generiert)" if f == "icons.h" else ""
                print(f"    include/{f}{GRAY_T}{gen}{RESET}")

    print(f"\n  {TEAL}Icons in icons/:{RESET}")
    pngs = sorted(f for f in os.listdir(ICONS_DIR)
                  if f.lower().endswith(".png")) if os.path.isdir(ICONS_DIR) else []
    if pngs:
        for f in pngs:
            name = os.path.splitext(f)[0].upper()
            print(f"    {f}  →  ICON_{name}")
    else:
        warn("Keine PNG-Dateien – Fallback-Icons aktiv")

    print(f"\n  {TEAL}Fallback-Icons (eingebettet):{RESET}")
    for name in sorted(FALLBACK_ICONS):
        overridden = os.path.exists(os.path.join(ICONS_DIR, name + ".png"))
        status = f"{GREEN}(PNG vorhanden){RESET}" if overridden \
                 else f"{GRAY_T}(Fallback){RESET}"
        print(f"    ICON_{name.upper():<14} {status}")
    print()


# ============================================================
# Einstiegspunkt
# ============================================================

if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "build"

    if cmd == "build":
        header("Build")
        info("Schritt 1/2: include/icons.h generieren")
        generate_icon_header()
        info("Schritt 2/2: OS kompilieren")
        if build_os():
            print(f"\n  {GREEN}{BOLD}✓  Build erfolgreich!{RESET}\n")
        else:
            print(f"\n  {RED_T}{BOLD}✗  Build fehlgeschlagen.{RESET}\n")
            sys.exit(1)

    elif cmd == "icons":
        header("Icons")
        generate_icon_header()

    elif cmd == "clean":
        header("Clean")
        clean()

    elif cmd == "info":
        show_info()

    else:
        print("Verwendung: python3 os_build.py [build|icons|clean|info]")
        sys.exit(1)