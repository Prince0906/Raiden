#include "renderer.h"
#include "screen.h"
#include "hud.h"
#include "bullet.h"
#include "config.h"

/* ── draw_boundary ───────────────────────────────────────────────────── */
/*
 * Draws a visible box around the play area using +, -, |.
 *
 *   +---------------------- ... ----+    <- BORDER_TOP
 *   |                               |
 *   |        (play area)            |
 *   |                               |
 *   +---------------------- ... ----+    <- BORDER_BOTTOM
 *   SCORE: 0                             <- HUD_ROW
 */
static void draw_boundary(void) {
    int i;

    /* Top and bottom horizontal edges */
    for (i = BORDER_LEFT; i <= BORDER_RIGHT; i++) {
        screen_draw_char(i, BORDER_TOP,    BORDER_HORIZ);
        screen_draw_char(i, BORDER_BOTTOM, BORDER_HORIZ);
    }

    /* Left and right vertical edges */
    for (i = BORDER_TOP; i <= BORDER_BOTTOM; i++) {
        screen_draw_char(BORDER_LEFT,  i, BORDER_VERT);
        screen_draw_char(BORDER_RIGHT, i, BORDER_VERT);
    }

    /* Four corners */
    screen_draw_char(BORDER_LEFT,  BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_LEFT,  BORDER_BOTTOM, BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_BOTTOM, BORDER_CORNER);
}

/* ── renderer_draw_frame ─────────────────────────────────────────────── */
/*
 * Z-order:
 *   1. Boundary box (background structure)
 *   2. Player plane (foreground)
 *   3. Score HUD    (below the box)
 */
void renderer_draw_frame(const Player *p) {
    screen_clear();

    draw_boundary();       /* layer 1: walls             */
    bullets_draw();        /* layer 2: falling bullets   */
    player_draw(p);        /* layer 3: plane (on top)    */
    hud_draw(p->health);   /* layer 4: score + HP bar  */

    screen_flip();
}