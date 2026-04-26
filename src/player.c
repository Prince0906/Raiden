#include "player.h"
#include "memory.h"
#include "screen.h"
#include "config.h"

/* ── player_init ─────────────────────────────────────────────────────── */
Player *player_init(void) {
    Player *p = (Player *)my_alloc((int)sizeof(Player));
    p->x          = PLAYER_START_X;
    p->y          = PLAYER_START_Y;
    p->health     = PLAYER_MAX_HEALTH;
    p->invincible = 0;
    return p;
}

/* ── player_move ─────────────────────────────────────────────────────── */
/*
 * Checks each direction bit independently — multiple can be set at once,
 * enabling true diagonal movement (e.g. KS_UP | KS_RIGHT = up-right).
 *
 * PLAYER_SPEED_X / _Y are separate because terminal cells are ~2× taller
 * than wide. Without this split, vertical movement looks faster than
 * horizontal even though both move the same number of cells per frame.
 */
void player_move(Player *p, KeyState ks) {
    if (ks & KS_UP)    p->y -= PLAYER_SPEED_Y;
    if (ks & KS_DOWN)  p->y += PLAYER_SPEED_Y;
    if (ks & KS_LEFT)  p->x -= PLAYER_SPEED_X;
    if (ks & KS_RIGHT) p->x += PLAYER_SPEED_X;

    p->x = my_clamp(p->x, PLAY_X_MIN + PLANE_HALF_W, PLAY_X_MAX - PLANE_HALF_W);
    p->y = my_clamp(p->y, PLAY_Y_MIN, PLAY_Y_MAX - PLANE_HEIGHT + 1);
}

/* ── player_update ───────────────────────────────────────────────────── */
/* Ticks the invincibility countdown by one frame. */
void player_update(Player *p) {
    if (p->invincible > 0) {
        p->invincible--;
    }
}

/* ── player_take_damage ──────────────────────────────────────────────── */
/*
 * Applies damage to the player only when not invincible.
 * On hit, starts the invincibility window so consecutive bullets from
 * the same collision zone don't drain the full health bar at once.
 */
void player_take_damage(Player *p, int dmg) {
    if (p->invincible > 0) return;      /* still immune — ignore this hit */

    p->health -= dmg;
    if (p->health < 0) p->health = 0;  /* clamp at 0, never negative      */

    p->invincible = PLAYER_INVINCIBLE_FRAMES;
}

/* ── player_is_dead ──────────────────────────────────────────────────── */
int player_is_dead(const Player *p) {
    return p->health <= 0;
}

void player_draw(const Player *p) {
    /*
     * Invincibility flash — rapid 6-frame cycle:
     *   invincible % 6 < 3  → draw   (visible  for 3 frames)
     *   invincible % 6 >= 3 → return (invisible for 3 frames)
     *
     * This replaces the old `% 40 < 20` logic which counted DOWN from
     * PLAYER_INVINCIBLE_FRAMES=40, making the plane invisible for the
     * entire last half of the window (values 19→1 all < 20 → skip).
     * The short 6-frame cycle flashes clearly throughout the full window.
     */
    if (p->invincible > 0 && (p->invincible % 6) >= 3) return;

    screen_draw_char(p->x,     p->y,     '^');
    screen_draw_char(p->x - 1, p->y + 1, '/');
    screen_draw_char(p->x,     p->y + 1, '|');
    screen_draw_char(p->x + 1, p->y + 1, '\\');
}