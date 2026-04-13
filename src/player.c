#include "player.h"
#include "memory.h"
#include "math.h"
#include "screen.h"
#include "config.h"

Player *player_init(void) {
    Player *p = (Player *)my_alloc((int)sizeof(Player));
    p->x          = PLAYER_START_X;
    p->y          = PLAYER_START_Y;
    p->health     = PLAYER_MAX_HEALTH;
    p->invincible = 0;
    return p;
}

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

void player_update(Player *p) {
    if (p->invincible > 0) {
        p->invincible--;
    }
}

void player_take_damage(Player *p, int dmg) {
    if (p->invincible > 0) return;      

    p->health -= dmg;
    if (p->health < 0) p->health = 0;  

    p->invincible = PLAYER_INVINCIBLE_FRAMES;
}

int player_is_dead(const Player *p) {
    return p->health <= 0;
}

void player_draw(const Player *p) {

    if (p->invincible > 0 && (p->invincible % 40) < 20) return;

    screen_draw_char(p->x,     p->y,     '^');
    screen_draw_char(p->x - 1, p->y + 1, '/');
    screen_draw_char(p->x,     p->y + 1, '|');
    screen_draw_char(p->x + 1, p->y + 1, '\\');
}