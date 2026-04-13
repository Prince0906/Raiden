#include "hud.h"
#include "screen.h"
#include "string.h"
#include "config.h"

static int score = 0;

void hud_init(void)      { score = 0; }
void hud_update(void)    { score++; }
int  hud_get_score(void) { return score; }

void hud_draw(int health) {
    char buf[INT_BUF_SIZE];
    int  filled;
    int  i;

    screen_draw_str(HUD_SCORE_COL, HUD_ROW, "SCORE:");
    int_to_str(score, buf, INT_BUF_SIZE);
    screen_draw_str(HUD_SCORE_COL + 7, HUD_ROW, buf);

    filled = health / 10;   

    screen_draw_str(50, HUD_ROW, "HP [");    
    for (i = 0; i < 10; i++) {
        screen_draw_char(54 + i, HUD_ROW, i < filled ? '=' : '-');
    }
    screen_draw_char(64, HUD_ROW, ']');

    int_to_str(health, buf, INT_BUF_SIZE);
    screen_draw_str(66, HUD_ROW, buf);
}