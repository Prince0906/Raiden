#include "player.h"
#include "memory.h"
#include "math.h"
#include "screen.h"
#include "config.h"

/* ── player_init ─────────────────────────────────────────────────────── */
Player *player_init(void) {
    Player *p = (Player *)my_alloc((int)sizeof(Player));
    p->x = PLAYER_START_X;
    p->y = PLAYER_START_Y;
    return p;
}

/* ── player_move ─────────────────────────────────────────────────────── */
/*
 * Clamp accounts for the plane's physical size:
 *   x ranges: [PLAY_X_MIN + PLANE_HALF_W .. PLAY_X_MAX - PLANE_HALF_W]
 *   y ranges: [PLAY_Y_MIN .. PLAY_Y_MAX - PLANE_HEIGHT + 1]
 * so no part of the plane overlaps the boundary walls.
 */
void player_move(Player *p, Key k) {
    switch (k) {
        case KEY_W: case KEY_UP:    p->y--; break;
        case KEY_S: case KEY_DOWN:  p->y++; break;
        case KEY_A: case KEY_LEFT:  p->x--; break;
        case KEY_D: case KEY_RIGHT: p->x++; break;
        default: break;
    }
    p->x = my_clamp(p->x, PLAY_X_MIN + PLANE_HALF_W, PLAY_X_MAX - PLANE_HALF_W);
    p->y = my_clamp(p->y, PLAY_Y_MIN, PLAY_Y_MAX - PLANE_HEIGHT + 1);
}

/* ── player_draw ─────────────────────────────────────────────────────── */
/*
 * The plane:
 *        ^          row y     (nose)
 *       /|\         row y+1   (wings + body)
 *
 * 3 chars wide, 2 rows tall. Drawn relative to centre (x, y).
 */
void player_draw(const Player *p) {
    /* Row 0: nose */
    screen_draw_char(p->x, p->y, '^');

    /* Row 1: wings + body */
    screen_draw_char(p->x - 1, p->y + 1, '/');
    screen_draw_char(p->x,     p->y + 1, '|');
    screen_draw_char(p->x + 1, p->y + 1, '\\');
}
