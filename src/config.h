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
#define BUSY_WAIT_ITERS      2000000

/* ── Bullets ──────────────────────────────────────────────────────────── */
#define MAX_ENEMY_BULLETS    20   /* pool shared by all enemy/hazard bullets */
#define BULLET_SPEED_FRAMES  40    /* frames between bullet move ticks     */
#define BULLET_SPAWN_FRAMES  0     /* 0 = disabled — all bullets from enemies */
#define BULLET_GLYPH         '*'
#define BULLET_DMG_SLOW      10   /* NOOB bullets — moderate damage          */
#define BULLET_DMG_FAST      25   /* MID spread bullets — punishing          */

/* ── Enemies ─────────────────────────────────────────────────────────── */
#define MAX_ENEMIES              6
#define ENEMY_NOOB_HEALTH        20
#define ENEMY_MID_HEALTH         50
#define ENEMY_MOVE_FRAMES        60    /* frames between enemy moving 1 row    */
#define ENEMY_SPAWN_INTERVAL     300   /* frames between new enemy spawns      */
#define ENEMY_NOOB_SHOOT_TIMER   100   /* frames between NOOB shots            */
#define ENEMY_MID_SHOOT_TIMER    180   /* frames between MID spread shots      */
#define ENEMY_NOOB_GLYPH         'V'
#define ENEMY_MID_GLYPH          'W'
#define ENEMY_SCORE_VALUE        100

/* ── Player health ────────────────────────────────────────────────────── */
#define PLAYER_MAX_HEALTH        100
#define PLAYER_INVINCIBLE_FRAMES 100  /* frames of immunity after taking a hit */

/* ── Scoring ─────────────────────────────────────────────────────────── */
#define SCORE_INTERVAL       60

/* ── Memory pool ─────────────────────────────────────────────────────── */
#define POOL_SIZE_BYTES     (64 * 1024)

/* ── Utility ─────────────────────────────────────────────────────────── */
#define INT_BUF_SIZE        12

#endif /* CONFIG_H */