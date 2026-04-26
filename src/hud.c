#include "hud.h"
#include "screen.h"
#include "string.h"
#include "config.h"

static int score = 0;

void hud_init(void)      { score = 0; }
void hud_update(void)    { score++; }
void hud_add_score(int n){ score += n; }
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
void hud_draw(int health, int weapon_level) {
    char buf[INT_BUF_SIZE];
    int  filled;
    int  i;

    /* ── Score ── */
    screen_draw_str(HUD_SCORE_COL, HUD_ROW, "SCORE:");
    int_to_str(score, buf, INT_BUF_SIZE);
    screen_draw_str(HUD_SCORE_COL + 7, HUD_ROW, buf);

    /* ── Weapon level ── */
    screen_draw_str(SCREEN_W / 2 - 4, HUD_ROW, "WPN:");
    int_to_str(weapon_level, buf, INT_BUF_SIZE);
    screen_draw_str(SCREEN_W / 2,     HUD_ROW, buf);

    /* ── Health bar ── */
    int hp_start = SCREEN_W - 30;
    filled = health / 10;

    screen_draw_str(hp_start, HUD_ROW, "HP [");
    for (i = 0; i < 10; i++) {
        screen_draw_char(hp_start + 4 + i, HUD_ROW, i < filled ? '=' : '-');
    }
    screen_draw_char(hp_start + 14, HUD_ROW, ']');

    int_to_str(health, buf, INT_BUF_SIZE);
    screen_draw_str(hp_start + 16, HUD_ROW, buf);
}