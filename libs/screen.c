#include "screen.h"
#include "math.h"
#include "memory.h"
#include "config.h"
#include <stdio.h>   /* printf, fflush — terminal I/O, explicitly allowed */

/*
 * Double-buffer anti-flicker design
 * ----------------------------------
 * front  = the character grid currently visible on screen
 * back   = the grid we are building this frame
 *
 * screen_flip() walks every cell. Where back[i] != front[i], it emits a
 * single ANSI cursor-move + character. The terminal only receives the
 * delta — typically < 20 bytes per frame — so it never sees a blank moment.
 *
 * Both buffers are allocated from the memory pool (not the stack) because
 * SCREEN_W * SCREEN_H = 1920 bytes each, and we want ownership to be
 * explicit and visible.
 */

#define BUF_SIZE  (SCREEN_W * SCREEN_H)

static char *front = 0;
static char *back  = 0;

/* ── screen_init ─────────────────────────────────────────────────────── */
void screen_init(void) {
    front = (char *)my_alloc(BUF_SIZE);
    back  = (char *)my_alloc(BUF_SIZE);

    /* Fill both buffers with spaces so first flip only emits real content */
    int i;
    for (i = 0; i < BUF_SIZE; i++) { front[i] = ' '; back[i] = ' '; }

    printf("\033[?25l");   /* hide cursor                   */
    printf("\033[2J");     /* erase entire screen            */
    printf("\033[H");      /* cursor to top-left             */
    fflush(stdout);
}

/* ── screen_clear ────────────────────────────────────────────────────── */
/* Called at the start of every frame to blank the back buffer. */
void screen_clear(void) {
    int i;
    for (i = 0; i < BUF_SIZE; i++) back[i] = ' ';
}

/* ── screen_draw_char ────────────────────────────────────────────────── */
void screen_draw_char(int x, int y, char c) {
    if (!in_bounds(x, 0, SCREEN_W - 1)) return;
    if (!in_bounds(y, 0, SCREEN_H - 1)) return;
    back[y * SCREEN_W + x] = c;
}

/* ── screen_draw_str ─────────────────────────────────────────────────── */
void screen_draw_str(int x, int y, const char *s) {
    int i = 0;
    while (s[i] != '\0') {
        screen_draw_char(x + i, y, s[i]);
        i++;
    }
}

/* ── screen_flip ─────────────────────────────────────────────────────── */
/*
 * Diffs front vs back. For every changed cell, emits:
 *   \033[row;colH<char>
 * ANSI row/col are 1-indexed. Then updates front to match back in-place,
 * so next frame's diff starts from the correct baseline.
 */
void screen_flip(void) {
    int i;
    for (i = 0; i < BUF_SIZE; i++) {
        if (back[i] != front[i]) {
            int row = i / SCREEN_W + 1;   /* 1-indexed for ANSI */
            int col = i % SCREEN_W + 1;
            printf("\033[%d;%dH%c", row, col, back[i]);
            front[i] = back[i];
        }
    }
    fflush(stdout);
}

/* ── screen_restore ──────────────────────────────────────────────────── */
void screen_restore(void) {
    printf("\033[%d;1H", SCREEN_H + 1);   /* cursor below game area */
    printf("\033[?25h");                   /* show cursor            */
    fflush(stdout);
}
