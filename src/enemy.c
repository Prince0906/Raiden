#include "enemy.h"
#include "bullet.h"
#include "screen.h"
#include "math.h"
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
            enemies[i].x      = enemy_rand_x();
            enemies[i].y      = PLAY_Y_MIN;
            enemies[i].active = 1;

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

    /* Phase 2: Movement */
    if (frame % ENEMY_MOVE_FRAMES == 0) {
        for (i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;
            enemies[i].y++;
            if (enemies[i].y > PLAY_Y_MAX) {
                enemies[i].active = 0;   /* flew off the bottom — free the slot */
            }
        }
    }

    /* Phase 3: Shooting */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        enemies[i].shoot_timer--;

        if (enemies[i].shoot_timer <= 0) {
            if (enemies[i].type == ENEMY_NOOB) {
                /*
                 * NOOB: single bullet fired straight down from nose position.
                 * dx=0 keeps it vertical; damage=BULLET_DMG_SLOW — easy to avoid.
                 */
                bullet_spawn(enemies[i].x, enemies[i].y + 1,
                             0, 1, BULLET_DMG_SLOW);
                enemies[i].shoot_timer = ENEMY_NOOB_SHOOT_TIMER;

            } else {
                /*
                 * MID: 3-bullet spread fan.
                 * dx offsets {-1,0,+1} create a tight widening cone.
                 * Fewer bullets but each is punishing damage.
                 */
                int s;
                int spreads[3] = {-1, 0, 1};
                for (s = 0; s < 3; s++) {
                    bullet_spawn(enemies[i].x,
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
