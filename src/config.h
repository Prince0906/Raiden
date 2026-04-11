#ifndef CONFIG_H
#define CONFIG_H

/* ── Screen ──────────────────────────────────────────────────────────── */
#define SCREEN_W            80
#define SCREEN_H            24

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

/* ── Boundary glyphs ─────────────────────────────────────────────────── */
#define BORDER_HORIZ        '-'
#define BORDER_VERT         '|'
#define BORDER_CORNER       '+'

/* ── HUD ─────────────────────────────────────────────────────────────── */
#define HUD_ROW             (SCREEN_H - 2)           /* row 22            */
#define HUD_SCORE_COL        2

/* ── Timing ──────────────────────────────────────────────────────────── */
#define BUSY_WAIT_ITERS     1000000

/* ── Memory pool ─────────────────────────────────────────────────────── */
#define POOL_SIZE_BYTES     (64 * 1024)

/* ── Utility ─────────────────────────────────────────────────────────── */
#define INT_BUF_SIZE        12

#endif /* CONFIG_H */
