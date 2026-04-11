#include "bullet.h"
#include "screen.h"
#include "math.h"
#include "config.h"

/*
 * ── LCG Random Number Generator ──────────────────────────────────────────
 * A Linear Congruential Generator (LCG) is the simplest viable pseudo-random
 * number source. It needs no <stdlib.h> rand() — forbidden for core logic.
 *
 * Formula: seed = seed * A + C  (mod 2^32, happens naturally with unsigned)
 * Constants A=1664525, C=1013904223 are the classic Numerical Recipes values.
 */
static unsigned int rng_seed = 73819u;

static unsigned int lcg_rand(void) {
    rng_seed = rng_seed * 1664525u + 1013904223u;
    return rng_seed;
}

/* Returns a random column inside the play area [PLAY_X_MIN .. PLAY_X_MAX]. */
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
    }
}

/* ── spawn_bullet ────────────────────────────────────────────────────── */
/* Finds the first free slot and places a bullet at the top of play area. */
static void spawn_bullet(void) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x      = rand_x();
            bullets[i].y      = PLAY_Y_MIN;
            bullets[i].active = 1;
            return;
        }
    }
    /* All slots full — skip spawn this cycle */
}

/* ── bullets_update ──────────────────────────────────────────────────── */
/*
 * Called every frame with the current frame counter.
 * Two independent timers:
 *   BULLET_SPAWN_FRAMES  — how often a new bullet appears
 *   BULLET_SPEED_FRAMES  — how often every active bullet drops 1 row
 *
 * Starting spawn at frame > 0 gives the player a moment to orient
 * before the first bullet appears.
 */
void bullets_update(unsigned int frame) {
    int i;

    /* Spawn: skip frame 0 so the player has a grace period */
    if (frame > 0 && frame % BULLET_SPAWN_FRAMES == 0) {
        spawn_bullet();
    }

    /* Move: drop every active bullet one row */
    if (frame % BULLET_SPEED_FRAMES == 0) {
        for (i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].y++;
                /* Deactivate when it exits the play area */
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

/* ── bullets_hit ─────────────────────────────────────────────────────── */
/*
 * The player plane occupies 4 cells:
 *
 *       (px,   py)          ← nose  ^
 *       (px-1, py+1)        ← left  /
 *       (px,   py+1)        ← body  |
 *       (px+1, py+1)        ← right \
 *
 * Returns 1 if any active bullet overlaps any of these cells.
 */
int bullets_hit(int px, int py) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        int bx = bullets[i].x;
        int by = bullets[i].y;
        if ((bx == px   && by == py    ) ||
            (bx == px-1 && by == py + 1) ||
            (bx == px   && by == py + 1) ||
            (bx == px+1 && by == py + 1)) {
            return 1;
        }
    }
    return 0;
}