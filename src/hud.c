#include "hud.h"
#include "screen.h"
#include "string.h"
#include "config.h"

static int score = 0;

void hud_init(void)      { score = 0; }
void hud_update(void)    { score++; }
int  hud_get_score(void) { return score; }

/* ── hud_draw ────────────────────────────────────────────────────────── */
/*
 * Renders two items on HUD_ROW (below the boundary box):
 *
 *   col 2  : SCORE: XXXX
 *   col 50 : HP [==========] 100
 *             ↑               ↑
 *             bar 10 chars    numeric value
 *
 * Health bar: filled cells = health / 10  (since health is 0..100)
 *   '=' = active health    '-' = lost health
 *
 * HUD layout on 80-col terminal:
 *  col: 2       9           50    54 55..64  65  67
 *       SCORE:  <score>     HP [  ==========  ]  <hp>
 */
void hud_draw(int health) {
    char buf[INT_BUF_SIZE];
    int  filled;
    int  i;

    /* ── Score ── */
    screen_draw_str(HUD_SCORE_COL, HUD_ROW, "SCORE:");
    int_to_str(score, buf, INT_BUF_SIZE);
    screen_draw_str(HUD_SCORE_COL + 7, HUD_ROW, buf);

    /* ── Health bar ──
     * "HP [==========] 100"
     *  col 50: H
     *  col 51: P
     *  col 52: (space)
     *  col 53: [
     *  col 54-63: bar (10 cells)
     *  col 64: ]
     *  col 65: (space — left blank by screen_clear)
     *  col 66-68: numeric hp value
     */
    filled = health / 10;   /* 0..10 */

    screen_draw_str(50, HUD_ROW, "HP [");    /* cols 50-53 */
    for (i = 0; i < 10; i++) {
        screen_draw_char(54 + i, HUD_ROW, i < filled ? '=' : '-');
    }
    screen_draw_char(64, HUD_ROW, ']');
}