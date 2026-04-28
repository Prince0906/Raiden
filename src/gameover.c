#include "gameover.h"
#include "config.h"
#include "keyboard.h"
#include "screen.h"
#include "string.h" /* int_to_str — custom */
#include <unistd.h> /* usleep */

/* ── draw_border ─────────────────────────────────────────────────────── */
static void draw_border(void) {
  int i;
  for (i = BORDER_LEFT; i <= BORDER_RIGHT; i++) {
    screen_draw_char(i, BORDER_TOP, BORDER_HORIZ);
    screen_draw_char(i, BORDER_BOTTOM, BORDER_HORIZ);
  }
  for (i = BORDER_TOP; i <= BORDER_BOTTOM; i++) {
    screen_draw_char(BORDER_LEFT, i, BORDER_VERT);
    screen_draw_char(BORDER_RIGHT, i, BORDER_VERT);
  }
  screen_draw_char(BORDER_LEFT, BORDER_TOP, BORDER_CORNER);
  screen_draw_char(BORDER_RIGHT, BORDER_TOP, BORDER_CORNER);
  screen_draw_char(BORDER_LEFT, BORDER_BOTTOM, BORDER_CORNER);
  screen_draw_char(BORDER_RIGHT, BORDER_BOTTOM, BORDER_CORNER);
}

/*
 * gameover_show — draws:
 *
 *   +------------------------------------------------------------------+
 *   |                                                                  |
 *   |                    *  G A M E   O V E R  *                      |
 *   |               ================================                   |
 *   |                                                                  |
 *   |                    FINAL SCORE :   4200                         |
 *   |                    YOUR RANK   :   #3                           |
 *   |                                                                  |
 *   |         [ R ] Replay    [ L ] Leaderboard    [ Q ] Quit         |
 *   |                                                                  |
 *   +------------------------------------------------------------------+
 *
 * Blocks until R / L / Q is pressed, then returns the corresponding state.
 */
GameState gameover_show(int score, int rank) {
  char score_buf[INT_BUF_SIZE];
  char rank_buf[INT_BUF_SIZE];

  int_to_str(score, score_buf, INT_BUF_SIZE);
  int_to_str(rank, rank_buf, INT_BUF_SIZE);

  int mid_y = (BORDER_TOP + BORDER_BOTTOM) / 2;
  int mid_x = SCREEN_W / 2;

  while (1) {
    screen_clear();
    draw_border();

    /* ── title ── */
    screen_draw_str(mid_x - 12, mid_y - 5, "*  G A M E   O V E R  *");
    screen_draw_str(mid_x - 16, mid_y - 4, "================================");

    /* ── score line ── */
    screen_draw_str(mid_x - 10, mid_y - 2, "FINAL SCORE :");
    screen_draw_str(mid_x + 4, mid_y - 2, score_buf);

    /* ── rank line ── */
    if (rank > 0) {
      screen_draw_str(mid_x - 10, mid_y - 1, "YOUR RANK   :  #");
      screen_draw_str(mid_x + 6, mid_y - 1, rank_buf);
    } else {
      screen_draw_str(mid_x - 10, mid_y - 1, "YOUR RANK   :  --");
    }

    /* ── action prompt ── */
    screen_draw_str(mid_x - 22, mid_y + 2,
                    "[ R ] Replay    [ L ] Leaderboard    [ Q ] Quit");

    screen_flip();

    /* poll for input */
    usleep(FRAME_US);
    Key k = kb_get_key();

    switch (k) {
    case KEY_Q:
      return STATE_QUIT;
    default:
      /* check raw character for R and L */
      if (k == 'r' || k == 'R')
        return STATE_MENU;
      if (k == 'l' || k == 'L')
        return STATE_LEADERBOARD;
      break;
    }
  }
}