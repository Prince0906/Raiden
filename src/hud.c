#include "hud.h"
#include "screen.h"
#include "string.h"
#include "config.h"

static int score = 0;

void hud_init(void)      { score = 0; }
void hud_update(void)    { score++; }
int  hud_get_score(void) { return score; }

/* ── hud_draw ────────────────────────────────────────────────────────── */
/* Renders only SCORE at HUD_ROW, below the boundary box. */
void hud_draw(void) {
    char buf[INT_BUF_SIZE];

    screen_draw_str(HUD_SCORE_COL, HUD_ROW, "SCORE:");
    int_to_str(score, buf, INT_BUF_SIZE);
    screen_draw_str(HUD_SCORE_COL + 7, HUD_ROW, buf);
}
