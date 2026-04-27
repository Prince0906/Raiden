#include "keyboard.h"
#include <stdio.h>     /* getchar — terminal input, explicitly allowed    */
#include <stdlib.h>    /* atexit — allowed for process control            */
#include <termios.h>   /* POSIX terminal control — hardware abstraction,
                          not a Standard C Library header                 */
#include <unistd.h>    /* usleep */

/*
 * Why termios?
 * ------------
 * The default terminal mode (cooked) buffers input until Enter, echos
 * characters, and makes getchar() block. A game loop needs none of this.
 *
 * Raw mode (VMIN=0, VTIME=0) makes getchar() return immediately:
 *   - EOF (-1) if no key is pressed this frame
 *   - the character code if a key was pressed
 *
 * termios is a POSIX hardware interface, not a Standard C Library header,
 * so it is permitted under the Hardware Abstraction Exception in the rules.
 * fd 0 (stdin) is used directly — STDIN_FILENO from <unistd.h> is not needed.
 */

static struct termios orig_termios;
static int kb_initialized = 0;

/* ── kb_restore ──────────────────────────────────────────────────────── */
/* Restores cooked mode. Registered via atexit so it runs even on crash. */
void kb_restore(void) {
    if (kb_initialized) {
        tcsetattr(0, TCSANOW, &orig_termios);
        kb_initialized = 0;
    }
}

/* ── kb_init ─────────────────────────────────────────────────────────── */
void kb_init(void) {
    tcgetattr(0, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= (unsigned int)~(ECHO | ICANON);  /* no echo, no line buf */
    raw.c_cc[VMIN]  = 0;   /* return immediately even if no bytes available */
    raw.c_cc[VTIME] = 0;   /* no timeout                                    */

    tcsetattr(0, TCSANOW, &raw);
    setvbuf(stdin, NULL, _IONBF, 0);  /* unbuffer stdin — critical for arrow keys */
    kb_initialized = 1;

    atexit(kb_restore);   /* safety net: restore terminal on any exit path  */
}

/* ── kb_get_key ──────────────────────────────────────────────────────── */
/*
 * Arrow keys send 3-byte escape sequences: ESC '[' + 'A'/'B'/'C'/'D'.
 * We read the first byte; if it is ESC (\033), we read two more bytes
 * to identify the full sequence. All reads use getchar() from <stdio.h>.
 */
Key kb_get_key(void) {
    clearerr(stdin);    /* reset sticky EOF flag from previous non-blocking read */
    int c = getchar();
    if (c == EOF) return KEY_NONE;

    switch (c) {
        case 'w': case 'W': return KEY_W;
        case 'a': case 'A': return KEY_A;
        case 's': case 'S': return KEY_S;
        case 'd': case 'D': return KEY_D;
        case ' ':           return KEY_SPACE;
        case 'q': case 'Q': return KEY_Q;
        case '\n': case '\r': return KEY_ENTER;
        case 127: case '\b': return KEY_BACKSPACE;
        case '\033': {
            int c2 = getchar();
            if (c2 == '[') {
                int c3 = getchar();
                switch (c3) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                    default:  return KEY_NONE;
                }
            }
            return KEY_NONE;
        }
        default: 
            if (c >= 32 && c <= 126) return c;
            return KEY_NONE;
    }
}

/* ── kb_drain_key ────────────────────────────────────────────────────── */
/*
 * Legacy single-key drain — still used by gameover.c's blocking input.
 * Reads every pending byte, keeps only the last.
 */
Key kb_drain_key(void) {
    Key last = KEY_NONE;
    Key k;
    while ((k = kb_get_key()) != KEY_NONE) {
        last = k;
    }
    return last;
}

/* ── kb_drain_keys ───────────────────────────────────────────────────── */
/*
 * V4 bitmask drain — reads ALL pending bytes and ORs each recognised key
 * into a KeyState bitmask. Multiple directions can be set simultaneously,
 * enabling diagonal movement.
 *
 * Why this works for "held" keys:
 *   When you hold W+D, the OS key-repeat sends alternating w,d,w,d chars
 *   into stdin. This loop reads all of them → both KS_UP and KS_RIGHT
 *   are set in the returned bitmask.
 *
 * Contradictory inputs (UP + DOWN) naturally cancel in player_move()
 * because the player position gets +speed then -speed on the same axis.
 */
KeyState kb_drain_keys(void) {
    KeyState state = 0;
    Key k;
    while ((k = kb_get_key()) != KEY_NONE) {
        switch (k) {
            case KEY_W: case KEY_UP:    state |= KS_UP;    break;
            case KEY_S: case KEY_DOWN:  state |= KS_DOWN;  break;
            case KEY_A: case KEY_LEFT:  state |= KS_LEFT;  break;
            case KEY_D: case KEY_RIGHT: state |= KS_RIGHT; break;
            case KEY_SPACE:             state |= KS_SPACE;  break;
            case KEY_Q:                 state |= KS_QUIT;   break;
            default: break;
        }
    }
    return state;
}

/* ── kb_read_string ──────────────────────────────────────────────────── */
char* kb_read_string(int max_len) {
    static char buf[256];
    int len = 0;
    buf[0] = 0;
    while (1) {
        Key k = kb_get_key();
        if (k == KEY_ENTER) break;
        if (k == KEY_BACKSPACE && len > 0) {
            len--;
            buf[len] = 0;
        } else if (k >= 32 && k <= 126 && len < max_len - 1) {  // printable chars
            buf[len++] = (char)k;
            buf[len] = 0;
        }
        usleep(10000);  // small delay to prevent tight loop
    }
    return buf;
}