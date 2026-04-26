#include "enemy.h"
#include "bullet.h"
#include "screen.h"
#include "config.h"

/*
 * LCG random — independent seed from bullet.c so enemy spawn X positions
 * are not correlated with bullet spawn X positions.
 */
static unsigned int enemy_rng_seed = 98273u;

static unsigned int enemy_lcg(void) {
    enemy_rng_seed = enemy_rng_seed * 1664525u + 1013904223u;
    return enemy_rng_seed;
}

static int enemy_rand_x(void) {
    int range = PLAY_X_MAX - PLAY_X_MIN + 1;
    return PLAY_X_MIN + (int)(enemy_lcg() % (unsigned int)range);
}

/* ── Enemy pool ──────────────────────────────────────────────────────── */
static Enemy enemies[MAX_ENEMIES];

/* ── spawn_enemy ─────────────────────────────────────────────────────── */
/*
 * Finds a free slot and initialises a new enemy at the top of the play area.
 * Type is chosen pseudo-randomly: even LCG value → NOOB, odd → MID.
 * If all slots are full the spawn cycle is skipped silently.
 */
static void spawn_enemy(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x          = enemy_rand_x();
            enemies[i].y          = PLAY_Y_MIN;
            enemies[i].active     = 1;
            enemies[i].move_timer = ENEMY_MOVE_FRAMES; /* own movement countdown */

            if (enemy_lcg() % 2 == 0) {
                enemies[i].type        = ENEMY_NOOB;
                enemies[i].health      = ENEMY_NOOB_HEALTH;
                enemies[i].shoot_timer = ENEMY_NOOB_SHOOT_TIMER;
            } else {
                enemies[i].type        = ENEMY_MID;
                enemies[i].health      = ENEMY_MID_HEALTH;
                enemies[i].shoot_timer = ENEMY_MID_SHOOT_TIMER;
            }
            return;
        }
    }
    /* All slots full — skip this spawn */
}

/* ── enemies_init ────────────────────────────────────────────────────── */
void enemies_init(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active      = 0;
        enemies[i].x           = 0;
        enemies[i].y           = 0;
        enemies[i].health      = 0;
        enemies[i].shoot_timer = 0;
        enemies[i].move_timer  = 0;
        enemies[i].type        = ENEMY_NOOB;
    }
}

/* ── enemies_update ──────────────────────────────────────────────────── */
/*
 * Three phases every frame:
 *   1. Maybe spawn a new enemy (every ENEMY_SPAWN_INTERVAL frames)
 *   2. Move all active enemies down (every ENEMY_MOVE_FRAMES frames)
 *   3. Tick shoot timers and fire bullets when the timer expires
 */
void enemies_update(unsigned int frame) {
    int i;

    /* Phase 1: Spawn */
    if (frame > 0 && frame % ENEMY_SPAWN_INTERVAL == 0) {
        spawn_enemy();
    }

    /*
     * Phases 2 + 3 merged into a single per-enemy loop.
     *
     * Phase 2 — Movement: each enemy owns a move_timer countdown so it
     * starts moving immediately after spawn instead of waiting for the next
     * global frame-modulo tick (which could freeze a newly spawned enemy at
     * the top of the screen for up to ENEMY_MOVE_FRAMES frames).
     *
     * Phase 3 — Shooting: unchanged; shoot_timer ticks every frame.
     */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        /* —— Move —— */
        enemies[i].move_timer--;
        if (enemies[i].move_timer <= 0) {
            enemies[i].y++;
            enemies[i].move_timer = ENEMY_MOVE_FRAMES;
            if (enemies[i].y > PLAY_Y_MAX) {
                enemies[i].active = 0;   /* flew off the bottom — free the slot */
                continue;
            }
        }

        /* —— Shoot —— */
        enemies[i].shoot_timer--;

        if (enemies[i].shoot_timer <= 0) {
            if (enemies[i].type == ENEMY_NOOB) {
                /*
                 * NOOB: single bullet fired straight down from nose position.
                 * dx=0 keeps it vertical; damage=BULLET_DMG_SLOW.
                 */
                bullet_spawn(enemies[i].x, enemies[i].y + 1,
                             0, 1, BULLET_DMG_SLOW);
                enemies[i].shoot_timer = ENEMY_NOOB_SHOOT_TIMER;

            } else {
                /*
                 * MID: 3-bullet spread fan.
                 * Each bullet spawns at its own x offset (x + dx) so all
                 * three are visually separated from the first frame.
                 * Previously all three started at the same cell, letting
                 * a player overlap take 3 × BULLET_DMG_FAST = 75 damage
                 * in a single frame.
                 */
                int s;
                int spreads[3] = {-1, 0, 1};
                for (s = 0; s < 3; s++) {
                    bullet_spawn(enemies[i].x + spreads[s],
                                 enemies[i].y + 1,
                                 spreads[s], 1, BULLET_DMG_FAST);
                }
                enemies[i].shoot_timer = ENEMY_MID_SHOOT_TIMER;
            }
        }
    }
}

/* ── enemies_draw ────────────────────────────────────────────────────── */
/*
 * NOOB drawn as 'V' — narrow, single shooter.
 * MID  drawn as 'W' — wide glyph matching its wide spread pattern.
 */
void enemies_draw(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        char glyph = (enemies[i].type == ENEMY_NOOB)
                     ? ENEMY_NOOB_GLYPH
                     : ENEMY_MID_GLYPH;
        screen_draw_char(enemies[i].x, enemies[i].y, glyph);
    }
}

/* ── enemies_check_hit ───────────────────────────────────────────────── */
/*
 * Checks whether any active enemy overlaps the player plane.
 *
 * Player hitbox (same 4 cells used by bullets_check_hit):
 *   (px,   py)      nose
 *   (px-1, py+1)    left wing
 *   (px,   py+1)    body
 *   (px+1, py+1)    right wing
 *
 * An enemy occupies a single cell (its x, y position).
 * On overlap: the enemy is deactivated (crash) and ENEMY_COLLISION_DAMAGE
 * is returned. The caller's invincibility window prevents repeated damage
 * across consecutive frames while the player steers away.
 *
 * Returns: ENEMY_COLLISION_DAMAGE if a collision occurred, else 0.
 */
int enemies_check_hit(int px, int py) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        int ex = enemies[i].x;
        int ey = enemies[i].y;
        if ((ex == px   && ey == py    ) ||
            (ex == px-1 && ey == py + 1) ||
            (ex == px   && ey == py + 1) ||
            (ex == px+1 && ey == py + 1)) {
            enemies[i].active = 0;   /* both planes crash */
            return ENEMY_COLLISION_DAMAGE;
        }
    }
    return 0;
}
