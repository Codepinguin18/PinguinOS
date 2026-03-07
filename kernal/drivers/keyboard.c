/**
 * @file keyboard.c
 * @brief PS/2 Tastatur-Treiber für PinguinOS – DE/US-Layout, IRQ1.
 */
#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"

/* ── Scancode-Tabellen (Set 1) ──────────────────────────────────── */
/* US-Layout: [Scancode] = {normal, shift} */
static const char us_map[128][2] = {
/*00*/ {0,0}, {0,0},  {'1','!'}, {'2','@'}, {'3','#'}, {'4','$'},
/*06*/ {'5','%'}, {'6','^'}, {'7','&'}, {'8','*'}, {'9','('},
/*11*/ {'0',')'}, {'-','_'}, {'=','+'}, {'\b','\b'}, {'\t','\t'},
/*15*/ {'q','Q'}, {'w','W'}, {'e','E'}, {'r','R'}, {'t','T'},
/*20*/ {'y','Y'}, {'u','U'}, {'i','I'}, {'o','O'}, {'p','P'},
/*25*/ {'[','{'}, {']','}'}, {'\n','\n'}, {0,0}, {'a','A'},
/*30*/ {'s','S'}, {'d','D'}, {'f','F'}, {'g','G'}, {'h','H'},
/*35*/ {'j','J'}, {'k','K'}, {'l','L'}, {';',':'}, {'\'','"'},
/*40*/ {'`','~'}, {0,0}, {'\\','|'}, {'z','Z'}, {'x','X'},
/*45*/ {'c','C'}, {'v','V'}, {'b','B'}, {'n','N'}, {'m','M'},
/*50*/ {',','<'}, {'.','>'}, {'/','?'}, {0,0}, {'*','*'},
/*55*/ {0,0}, {' ',' '}, {0,0},
};

/* DE-Layout (QWERTZ): [Scancode] = {normal, shift} */
static const char de_map[128][2] = {
/*00*/ {0,0}, {0,0},  {'1','!'}, {'2','"'}, {'3',0},   {'4','$'},
/*06*/ {'5','%'}, {'6','&'}, {'7','/'}, {'8','('}, {'9',')'},
/*11*/ {'0','='}, {0,0},    {0,0},    {'\b','\b'}, {'\t','\t'},
/*15*/ {'q','Q'}, {'w','W'}, {'e','E'}, {'r','R'}, {'t','T'},
/*20*/ {'z','Z'}, {'u','U'}, {'i','I'}, {'o','O'}, {'p','P'},
/*25*/ {0,0},    {'+','*'}, {'\n','\n'}, {0,0}, {'a','A'},
/*30*/ {'s','S'}, {'d','D'}, {'f','F'}, {'g','G'}, {'h','H'},
/*35*/ {'j','J'}, {'k','K'}, {'l','L'}, {0,0},    {0,0},
/*40*/ {'^',0},   {0,0},    {'#','\''},{'y','Y'}, {'x','X'},
/*45*/ {'c','C'}, {'v','V'}, {'b','B'}, {'n','N'}, {'m','M'},
/*50*/ {',',';'}, {'.',':'}, {'-','_'}, {0,0},    {'*','*'},
/*55*/ {0,0},    {' ',' '}, {0,0},
};

/* ── Modulzustand ────────────────────────────────────────────────── */
static kbd_event_t  kbd_buf[KBD_BUFFER_SIZE];
static uint32_t     kbd_head = 0;
static uint32_t     kbd_tail = 0;
static uint8_t      modifiers = 0;
static kbd_layout_t layout    = KBD_LAYOUT_US;

/* ── Puffer-Operationen ──────────────────────────────────────────── */
static void buf_push(kbd_event_t *ev)
{
    uint32_t next = (kbd_tail + 1) % KBD_BUFFER_SIZE;
    if (next != kbd_head) {
        kbd_buf[kbd_tail] = *ev;
        kbd_tail = next;
    }
}

static bool buf_pop(kbd_event_t *ev)
{
    if (kbd_head == kbd_tail) return false;
    *ev = kbd_buf[kbd_head];
    kbd_head = (kbd_head + 1) % KBD_BUFFER_SIZE;
    return true;
}

/* ── IRQ1-Handler ────────────────────────────────────────────────── */
static void kbd_irq_handler(cpu_regs_t *regs)
{
    (void)regs;

    /* Warten bis Daten bereit */
    if (!(inb(PS2_STATUS_PORT) & PS2_STATUS_OBF)) return;

    uint8_t sc = inb(PS2_DATA_PORT);
    bool released = (sc & 0x80) != 0;
    sc &= 0x7F;

    kbd_event_t ev = { .scancode = sc, .released = released,
                       .ascii = 0, .modifiers = modifiers };

    /* Modifier-Tasten verwalten */
    switch (sc) {
    case KEY_LSHIFT: case KEY_RSHIFT:
        if (released) modifiers &= ~KBD_MOD_SHIFT;
        else          modifiers |=  KBD_MOD_SHIFT;
        return;
    case KEY_LCTRL:
        if (released) modifiers &= ~KBD_MOD_CTRL;
        else          modifiers |=  KBD_MOD_CTRL;
        return;
    case KEY_LALT:
        if (released) modifiers &= ~KBD_MOD_ALT;
        else          modifiers |=  KBD_MOD_ALT;
        return;
    case KEY_CAPS:
        if (!released) modifiers ^= KBD_MOD_CAPS;
        return;
    }

    if (released) return;   /* Nur Key-Down-Events weitergeben */

    /* ASCII übersetzen */
    if (sc < 128) {
        const char (*map)[2] = (layout == KBD_LAYOUT_DE) ? de_map : us_map;
        bool shifted = (modifiers & KBD_MOD_SHIFT) ||
                       (modifiers & KBD_MOD_CAPS);
        ev.ascii     = map[sc][shifted ? 1 : 0];
    }

    ev.modifiers = modifiers;
    buf_push(&ev);
}

/* ── Öffentliche API ─────────────────────────────────────────────── */
void kbd_init(void)
{
    kbd_head  = kbd_tail = 0;
    modifiers = 0;
    layout    = KBD_LAYOUT_US;

    irq_register(IRQ_KEYBOARD, kbd_irq_handler);
    KINFO("Keyboard: PS/2-Treiber aktiv (IRQ1)\n");
}

void kbd_set_layout(kbd_layout_t l)
{
    layout = l;
    KINFO("Keyboard: Layout → %s\n", l == KBD_LAYOUT_DE ? "DE" : "US");
}

bool kbd_poll(kbd_event_t *event)    { return buf_pop(event); }
bool kbd_has_input(void)             { return kbd_head != kbd_tail; }

char kbd_getchar(void)
{
    kbd_event_t ev;
    while (!kbd_poll(&ev)) task_yield();
    return ev.ascii;
}
