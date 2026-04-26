#include "gameover.h"
#include "screen.h"
#include "keyboard.h"
#include "string.h"   /* int_to_str — our custom string.h */
#include "hud.h"
#include "config.h"
#include <unistd.h>   /* usleep */

/* Poll interval while waiting on the game-over screen for Q press. */
static void go_wait(void) {
    usleep(FRAME_US);
}

/* ── gameover_show ───────────────────────────────────────────────────── */
/*
 * Draws a centred panel inside the boundary box:
 *
 *   +--------------------------------------+
 *   |                                      |
 *   |           * GAME  OVER *             |
 *   |         ----------------------       |
 *   |           SCORE: <final>             |
 *   |                                      |
 *   |           [ Q ] to exit              |
 *   |                                      |
 *   +--------------------------------------+
 */
void gameover_show(void) {
    char buf[INT_BUF_SIZE];
    int  mid_y = (BORDER_TOP + BORDER_BOTTOM) / 2;
    int  mid_x = SCREEN_W / 2;
    int  i;

    screen_clear();

    /* ── Redraw the boundary box ── */
    for (i = BORDER_LEFT; i <= BORDER_RIGHT; i++) {
        screen_draw_char(i, BORDER_TOP,    BORDER_HORIZ);
        screen_draw_char(i, BORDER_BOTTOM, BORDER_HORIZ);
    }
    for (i = BORDER_TOP; i <= BORDER_BOTTOM; i++) {
        screen_draw_char(BORDER_LEFT,  i, BORDER_VERT);
        screen_draw_char(BORDER_RIGHT, i, BORDER_VERT);
    }
    screen_draw_char(BORDER_LEFT,  BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_LEFT,  BORDER_BOTTOM, BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_BOTTOM, BORDER_CORNER);

    /* ── Game over text ── */
    screen_draw_str(mid_x - 7,  mid_y - 2, "* GAME OVER *");
    screen_draw_str(mid_x - 10, mid_y - 1, "----------------------");

    /* ── Final score ── */
    int_to_str(hud_get_score(), buf, INT_BUF_SIZE);
    screen_draw_str(mid_x - 7, mid_y, "SCORE: ");
    screen_draw_str(mid_x,     mid_y, buf);

    /* ── Exit prompt ── */
    screen_draw_str(mid_x - 6, mid_y + 2, "[ Q ] to exit");

    screen_flip();

    /* ── Block until Q ── */
    {
        Key k = KEY_NONE;
        while (k != KEY_Q) {
            go_wait();
            k = kb_get_key();
        }
    }
}