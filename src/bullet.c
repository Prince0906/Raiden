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
static Bullet bullets[MAX_ENEMY_BULLETS];

/* ── bullets_init ────────────────────────────────────────────────────── */
void bullets_init(void) {
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i].active     = 0;
        bullets[i].x          = 0;
        bullets[i].y          = 0;
        bullets[i].damage     = 0;
        bullets[i].dx         = 0;
        bullets[i].dy         = 0;
        bullets[i].move_timer = 0;
    }
}

/* ── bullet_spawn ────────────────────────────────────────────────────── */
/*
 * Public API — used by enemies (and the future player) to fire bullets.
 * Searches the pool for a free slot (first-fit). Returns 1 on success,
 * 0 if the pool is full (caller can safely ignore the return value).
 */
int bullet_spawn(int x, int y, int dx, int dy, int damage) {
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x          = x;
            bullets[i].y          = y;
            bullets[i].dx         = dx;
            bullets[i].dy         = dy;
            bullets[i].active     = 1;
            bullets[i].damage     = damage;
            bullets[i].move_timer = BULLET_SPEED_FRAMES; /* own countdown */
            return 1;
        }
    }
    return 0;  /* pool full — spawn skipped silently */
}

/* ── spawn_bullet ────────────────────────────────────────────────────── */
/*
 * Internal: fires a random hazard bullet at a random X on the top row.
 * These are background environmental bullets — slow and individually weak.
 * Delegates to bullet_spawn() so slot-finding logic lives in one place.
 */
static void spawn_bullet(void) {
    bullet_spawn(rand_x(), PLAY_Y_MIN, 0, 1, BULLET_DMG_SLOW);
}

/* ── bullets_update ──────────────────────────────────────────────────── */
void bullets_update(unsigned int frame) {
    int i;

    /* Optional environmental bullet spawner (currently disabled). */
    if (BULLET_SPAWN_FRAMES > 0 && frame > 0 && frame % BULLET_SPAWN_FRAMES == 0) {
        spawn_bullet();
    }

    /*
     * Per-bullet move timer — each bullet tracks its own countdown so
     * bullets fire uniformly regardless of what frame they were spawned on.
     * Previously a global `frame % BULLET_SPEED_FRAMES` tick was used,
     * which made bullets spawned mid-interval wait different amounts before
     * their first move — causing visibly unequal bullet speeds.
     */
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].active) continue;

        bullets[i].move_timer--;
        if (bullets[i].move_timer <= 0) {
            bullets[i].x += bullets[i].dx;
            bullets[i].y += bullets[i].dy;
            bullets[i].move_timer = BULLET_SPEED_FRAMES; /* reset for next tick */

            /*
             * Deactivate if bullet leaves the play area in any direction.
             * y < PLAY_Y_MIN handles future upward player bullets (V5).
             */
            if (bullets[i].y > PLAY_Y_MAX ||
                bullets[i].y < PLAY_Y_MIN  ||
                !in_bounds(bullets[i].x, PLAY_X_MIN, PLAY_X_MAX)) {
                bullets[i].active = 0;
            }
        }
    }
}

/* ── bullets_draw ────────────────────────────────────────────────────── */
void bullets_draw(void) {
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (bullets[i].active) {
            screen_draw_char(bullets[i].x, bullets[i].y, BULLET_GLYPH);
        }
    }
}

/* ── bullets_check_hit ───────────────────────────────────────────────── */
/*
 * Checks all 4 plane cells against every active bullet.
 * EVERY overlapping bullet is consumed (deactivated) and its damage is
 * accumulated. This prevents "phantom" bullets that visually sit on top
 * of the player but don't register because only one was checked per frame.
 *
 * Returns: total damage from all bullets that hit this frame (>= 0)
 *
 * Plane occupies:
 *   (px,   py)     nose
 *   (px-1, py+1)   left wing
 *   (px,   py+1)   body
 *   (px+1, py+1)   right wing
 */
int bullets_check_hit(int px, int py) {
    int total_dmg = 0;
    int i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].active) continue;
        int bx = bullets[i].x;
        int by = bullets[i].y;
        if ((bx == px   && by == py    ) ||
            (bx == px-1 && by == py + 1) ||
            (bx == px   && by == py + 1) ||
            (bx == px+1 && by == py + 1)) {
            total_dmg += bullets[i].damage;
            bullets[i].active = 0;   /* consume the bullet */
        }
    }
    return total_dmg;
}