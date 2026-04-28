#include "menu.h"
#include "config.h"
#include "keyboard.h"
#include "screen.h"
#include "string.h"
#include "user.h"
#include <unistd.h> /* usleep */

/*
 * menu_show — navigable main menu.
 *
 * Options:
 *   0  START GAME
 *   1  LEADERBOARD
 *   2  CHANGE NAME
 *   3  QUIT
 *
 * W / UP   : move cursor up
 * S / DOWN : move cursor down
 * ENTER    : select
 * Q        : quit immediately
 */

#define NUM_ITEMS 4

static const char *items[NUM_ITEMS] = {"START GAME", "LEADERBOARD",
                                       "CHANGE NAME", "QUIT"};

static const GameState item_states[NUM_ITEMS] = {
    STATE_PLAYING, STATE_LEADERBOARD, STATE_USERNAME, STATE_QUIT};

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

/* ── menu_show ───────────────────────────────────────────────────────── */
GameState menu_show(void) {
  int cursor = 0;

  while (1) {
    screen_clear();
    draw_border();

    int mid_x = SCREEN_W / 2;
    int mid_y = SCREEN_H / 2;

    /* ── RAIDEN logo (small) ── */
    screen_draw_str(mid_x - 3, mid_y - 8, "RAIDEN");
    screen_draw_str(mid_x - 3, mid_y - 7, "======");

    /* ── Welcome line ── */
    User *u = user_get_current();
    if (u->name[0] != '\0') {
      /* build "Welcome,  <name>!" without sprintf */
      char line[80];
      int pos = 0;
      const char *prefix = "Welcome back,  ";
      const char *suffix = "!";
      int pi = 0;
      while (prefix[pi] && pos < 79)
        line[pos++] = prefix[pi++];
      int ni = 0;
      while (u->name[ni] && pos < 79)
        line[pos++] = u->name[ni++];
      int si = 0;
      while (suffix[si] && pos < 79)
        line[pos++] = suffix[si++];
      line[pos] = '\0';
      screen_draw_str(mid_x - my_strlen(line) / 2, mid_y - 5, line);
    }

    /* ── divider ── */
    screen_draw_str(mid_x - 10, mid_y - 4, "- - - - - - - - - - - -");

    /* ── menu items ── */
    int i;
    for (i = 0; i < NUM_ITEMS; i++) {
      int item_y = mid_y - 2 + i * 2;
      if (i == cursor) {
        screen_draw_str(mid_x - 12, item_y, "> ");
        screen_draw_str(mid_x - 10, item_y, items[i]);
      } else {
        screen_draw_str(mid_x - 10, item_y, items[i]);
      }
    }

    /* ── controls hint ── */
    screen_draw_str(mid_x - 18, BORDER_BOTTOM - 2,
                    "W/S or Arrows: Navigate    ENTER: Select    Q: Quit");

    screen_flip();

    /* wait for input */
    Key k = KEY_NONE;
    while (k == KEY_NONE) {
      usleep(10000);
      k = kb_get_key();
    }

    switch (k) {
    case KEY_W:
    case KEY_UP:
      cursor = (cursor - 1 + NUM_ITEMS) % NUM_ITEMS;
      break;
    case KEY_S:
    case KEY_DOWN:
      cursor = (cursor + 1) % NUM_ITEMS;
      break;
    case KEY_ENTER:
      return item_states[cursor];
    case KEY_Q:
      return STATE_QUIT;
    default:
      break;
    }
  }
}