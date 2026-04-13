#include "bullet.h"
#include "screen.h"
#include "math.h"
#include "config.h"

/*
 * LCG random number generator — no <stdlib.h> rand().
 * seed = seed * A + C (mod 2^32, wraps naturally with unsigned int)
 */
static unsigned int rng_seed = 73819u;

static unsigned int lcg_rand(void) {
    rng_seed = rng_seed * 1664525u + 1013904223u;
    return rng_seed;
}

static int rand_x(void) {
    int range = PLAY_X_MAX - PLAY_X_MIN + 1;
    return PLAY_X_MIN + (int)(lcg_rand() % (unsigned int)range);
}

/* ── Bullet pool ─────────────────────────────────────────────────────── */
static Bullet bullets[MAX_BULLETS];

/* ── bullets_init ────────────────────────────────────────────────────── */
void bullets_init(void) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].x      = 0;
        bullets[i].y      = 0;
        bullets[i].damage = 0;
    }
}

/* ── spawn_bullet ────────────────────────────────────────────────────── */
static void spawn_bullet(void) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x      = rand_x();
            bullets[i].y      = PLAY_Y_MIN;
            bullets[i].active = 1;
            bullets[i].damage = BULLET_DMG_DEFAULT;  /* current single type */
            return;
        }
    }
    /* All slots full — skip this spawn cycle */
}

/* ── bullets_update ──────────────────────────────────────────────────── */
void bullets_update(unsigned int frame) {
    int i;

    if (frame > 0 && frame % BULLET_SPAWN_FRAMES == 0) {
        spawn_bullet();
    }

    if (frame % BULLET_SPEED_FRAMES == 0) {
        for (i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].y++;
                if (bullets[i].y > PLAY_Y_MAX) {
                    bullets[i].active = 0;
                }
            }
        }
    }
}

/* ── bullets_draw ────────────────────────────────────────────────────── */
void bullets_draw(void) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            screen_draw_char(bullets[i].x, bullets[i].y, BULLET_GLYPH);
        }
    }
}

/* ── bullets_check_hit ───────────────────────────────────────────────── */
/*
 * Checks all 4 plane cells against every active bullet.
 * If a hit is found the bullet is DEACTIVATED (consumed) so it cannot
 * deal damage across multiple frames while overlapping the plane.
 *
 * Returns: damage value of the bullet that hit (> 0 = hit occurred)
 *          0 = no hit this frame
 *
 * Plane occupies:
 *   (px,   py)     nose
 *   (px-1, py+1)   left wing
 *   (px,   py+1)   body
 *   (px+1, py+1)   right wing
 */
int bullets_check_hit(int px, int py) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        int bx = bullets[i].x;
        int by = bullets[i].y;
        if ((bx == px   && by == py    ) ||
            (bx == px-1 && by == py + 1) ||
            (bx == px   && by == py + 1) ||
            (bx == px+1 && by == py + 1)) {
            int dmg = bullets[i].damage;
            bullets[i].active = 0;   /* consume the bullet */
            return dmg;
        }
    }
    return 0;
}