#include "renderer.h"
#include "screen.h"
#include "hud.h"
#include "bullet.h"
#include "config.h"

static void draw_boundary(void) {
    int i;

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
}

void renderer_draw_frame(const Player *p) {
    screen_clear();

    draw_boundary();       
    bullets_draw();        
    player_draw(p);        
    hud_draw(p->health);   

    screen_flip();
}