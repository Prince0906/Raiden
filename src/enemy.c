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
    /* MIDs are 5 cells wide (center +/- 2), so margin is 2 to stay in bounds */
    int min_x = PLAY_X_MIN + 2;
    int max_x = PLAY_X_MAX - 2;
    int range = max_x - min_x + 1;
    return min_x + (int)(enemy_lcg() % (unsigned int)range);
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
                             0, 1, BULLET_DMG_SLOW, 0);
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
                                 spreads[s], 1, BULLET_DMG_FAST, 0);
                }
                enemies[i].shoot_timer = ENEMY_MID_SHOOT_TIMER;
            }
        }
    }
}

/* ── enemies_draw ────────────────────────────────────────────────────── */
/*
 * NOOB drawn as '/V\'   (3 cells wide)
 * MID  drawn as '/-W-\' (5 cells wide)
 */
void enemies_draw(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        if (enemies[i].type == ENEMY_NOOB) {
            screen_draw_char(enemies[i].x - 1, enemies[i].y, '/');
            screen_draw_char(enemies[i].x,     enemies[i].y, ENEMY_NOOB_GLYPH);
            screen_draw_char(enemies[i].x + 1, enemies[i].y, '\\');
        } else {
            screen_draw_char(enemies[i].x - 2, enemies[i].y, '/');
            screen_draw_char(enemies[i].x - 1, enemies[i].y, '-');
            screen_draw_char(enemies[i].x,     enemies[i].y, ENEMY_MID_GLYPH);
            screen_draw_char(enemies[i].x + 1, enemies[i].y, '-');
            screen_draw_char(enemies[i].x + 2, enemies[i].y, '\\');
        }
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
 * NOOB occupies 3 cells (x-1 to x+1). MID occupies 5 cells (x-2 to x+2).
 * On overlap: the enemy is deactivated (crash) and ENEMY_COLLISION_DAMAGE
 * is returned.
 *
 * Returns: ENEMY_COLLISION_DAMAGE if a collision occurred, else 0.
 */
int enemies_check_hit(int px, int py) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        int w = (enemies[i].type == ENEMY_NOOB) ? 1 : 2;
        int ey = enemies[i].y;
        int dx;
        
        for (dx = -w; dx <= w; dx++) {
            int ex = enemies[i].x + dx;
            if ((ex == px   && ey == py    ) ||
                (ex == px-1 && ey == py + 1) ||
                (ex == px   && ey == py + 1) ||
                (ex == px+1 && ey == py + 1)) {
                
                enemies[i].active = 0;   /* both planes crash */
                return ENEMY_COLLISION_DAMAGE;
            }
        }
    }
    return 0;
}

/* ── enemies_process_player_bullets ─────────────────────────────────── */
/*
 * Called once per frame from main.c AFTER bullets_update.
 * For each active enemy, asks bullet.c if any player bullet (is_player==1)
 * overlaps its position (accounting for wider plane bodies). On hit:
 *   • bullet deactivated (inside bullets_consume_player_hits)
 *   • enemy health reduced
 *   • if health ≤ 0: enemy deactivated, ENEMY_SCORE_VALUE added to return
 *
 * Returns total score earned from kills this frame.
 */
int enemies_process_player_bullets(void) {
    int i, total_score = 0;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        int w = (enemies[i].type == ENEMY_NOOB) ? 1 : 2;
        int dx;
        int dmg = 0;
        
        for (dx = -w; dx <= w; dx++) {
            dmg += bullets_consume_player_hits(enemies[i].x + dx, enemies[i].y);
        }
        
        if (dmg > 0) {
            enemies[i].health -= dmg;
            if (enemies[i].health <= 0) {
                enemies[i].active  = 0;
                total_score       += ENEMY_SCORE_VALUE;
            }
        }
    }
    return total_score;
}
