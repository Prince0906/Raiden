#ifndef CONFIG_H
#define CONFIG_H

/* ── Screen ──────────────────────────────────────────────────────────── */
#define SCREEN_W            100
#define SCREEN_H            30

/* ── Boundary box (visible walls) ────────────────────────────────────── */
#define BORDER_TOP           0
#define BORDER_BOTTOM       (SCREEN_H - 3)          /* row 21            */
#define BORDER_LEFT          0
#define BORDER_RIGHT        (SCREEN_W - 1)           /* col 79            */

/* ── Play area (inside the walls) ────────────────────────────────────── */
#define PLAY_X_MIN          (BORDER_LEFT + 1)        /* col 1             */
#define PLAY_X_MAX          (BORDER_RIGHT - 1)       /* col 78            */
#define PLAY_Y_MIN          (BORDER_TOP + 1)         /* row 1             */
#define PLAY_Y_MAX          (BORDER_BOTTOM - 1)      /* row 20            */

/* ── Player plane ────────────────────────────────────────────────────── */
/*
 * The plane is 3 chars wide, 2 rows tall:
 *        ^          row y
 *       /|\         row y+1
 *
 * START position is centre of play area.
 * Clamp offsets account for the plane's physical size.
 */
#define PLAYER_START_X      (SCREEN_W / 2)
#define PLAYER_START_Y      (PLAY_Y_MAX - 3)         /* near bottom       */
#define PLANE_HALF_W         1                        /* 1 cell left/right of centre */
#define PLANE_HEIGHT         2                        /* 2 rows tall       */
#define PLAYER_SPEED_X  4                        /* horizontal: wider cells need more steps */
#define PLAYER_SPEED_Y  2                        /* vertical:   taller cells need fewer    */

/* ── Boundary glyphs ─────────────────────────────────────────────────── */
#define BORDER_HORIZ        '-'
#define BORDER_VERT         '|'
#define BORDER_CORNER       '+'

/* ── HUD ─────────────────────────────────────────────────────────────── */
#define HUD_ROW             (SCREEN_H - 2)           /* row 22            */
#define HUD_SCORE_COL        2

/* ── Timing ──────────────────────────────────────────────────────────── */
/*
 * busy_wait was replaced by usleep(FRAME_US) in main.c.
 * BUSY_WAIT_ITERS was a spin-loop whose duration was fully CPU-dependent
 * (e.g. ~2 ms on an M2, ~10 ms on slower hardware) — not 50 ms as intended.
 * All frame-count constants below are calibrated for TARGET_FPS = 20,
 * i.e. one frame = FRAME_US = 50 000 µs = 50 ms.
 */
#define TARGET_FPS           30
#define FRAME_US             (1000000 / TARGET_FPS)   /* ~33 333 µs per frame = 30 FPS */

/* ── Bullets ──────────────────────────────────────────────────────────── */
#define MAX_ENEMY_BULLETS    30   /* shared pool: enemy + player bullets      */
#define BULLET_SPEED_FRAMES  2    /* 30 FPS: 100 ms/cell, ~2.5 s to cross    */
#define BULLET_SPAWN_FRAMES  0    /* 0 = disabled — all bullets from enemies  */
#define BULLET_GLYPH         '*'  /* enemy bullet glyph                       */
#define PLAYER_BULLET_GLYPH  '|'  /* player bullet glyph                      */
#define BULLET_DMG_SLOW      10   /* NOOB bullets — moderate damage            */
#define BULLET_DMG_FAST      25   /* MID spread bullets — punishing            */

/* ── Player shooting (V5) ────────────────────────────────────────────── */
#define PLAYER_BULLET_DAMAGE   20  /* base HP removed per player bullet       */
#define PLAYER_SHOOT_COOLDOWN   6  /* 30 FPS: 200 ms between shots            */
#define MAX_WEAPON_LEVEL        3  /* 0=single 1=boosted 2=triple 3=wide      */

/* ── Enemies ─────────────────────────────────────────────────────────── */
#define MAX_ENEMIES              6
#define ENEMY_NOOB_HEALTH        20
#define ENEMY_MID_HEALTH         50
#define ENEMY_MOVE_FRAMES        4    /* 20 FPS: 200 ms/row, exits screen in ~5 s   */
#define ENEMY_SPAWN_INTERVAL     60   /* 20 FPS: new enemy every 3 seconds           */
#define ENEMY_NOOB_SHOOT_TIMER   15   /* 20 FPS: single shot every 0.75 s            */
#define ENEMY_MID_SHOOT_TIMER    25   /* 20 FPS: spread salvo every 1.25 s           */
#define ENEMY_NOOB_GLYPH         'V'
#define ENEMY_MID_GLYPH          'W'
#define ENEMY_SCORE_VALUE        100
#define ENEMY_COLLISION_DAMAGE   25   /* HP lost when player body overlaps enemy     */

/* ── Player health ────────────────────────────────────────────────────── */
#define PLAYER_MAX_HEALTH        100
#define PLAYER_INVINCIBLE_FRAMES 30   /* 30 FPS: 1 second of post-hit immunity, flashes visibly */

/* ── Scoring ─────────────────────────────────────────────────────────── */
#define SCORE_INTERVAL       20   /* 20 FPS: score increments once per second */

/* ── Memory pool ─────────────────────────────────────────────────────── */
#define POOL_SIZE_BYTES     (64 * 1024)

/* ── Utility ─────────────────────────────────────────────────────────── */
#define INT_BUF_SIZE        12

#endif /* CONFIG_H */