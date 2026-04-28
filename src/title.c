#include "title.h"
#include "config.h"
#include "keyboard.h"
#include "screen.h"
#include "user.h"
#include <unistd.h> /* usleep */

/*
 * title_show — draws the RAIDEN ASCII-art banner and waits for any key.
 *
 * The banner is 6 rows tall, centred horizontally inside SCREEN_W (100).
 * Each letter is hand-drawn as strings, placed row by row.
 *
 * Layout (inside the 100x30 box):
 *
 *   row  3-8   : ASCII-art RAIDEN letters
 *   row 10     : subtitle line
 *   row 12     : animated prompt
 *   row 15-17  : controls reminder
 *   row 20-22  : small decorative star-field strip
 */

/* ── 5-row ASCII-art letters for "RAIDEN" ────────────────────────────── */
/* Each letter is 9 chars wide + 1 space gap. Total: 6 letters × 10 = 60 chars.
   Centred in SCREEN_W=100 → left margin = (100-60)/2 = 20               */

static const char *banner[6][6] = {
    /* R */
    {" ######  ", " #    #  ", " ######  ", " # \\     ", " #  \\ #  ",
     "          "},
    /* A */
    {"  ####   ", " #    #  ", " ######  ", " #    #  ", " #    #  ",
     "          "},
    /* I */
    {" ######  ", "   ##    ", "   ##    ", "   ##    ", " ######  ",
     "          "},
    /* D */
    {" #####   ", " #    #  ", " #    #  ", " #    #  ", " #####   ",
     "          "},
    /* E */
    {" ######  ", " #       ", " #####   ", " #       ", " ######  ",
     "          "},
    /* N */
    {" #    #  ", " ##   #  ", " # #  #  ", " #  # #  ", " #   ##  ",
     "          "}};

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

/* ── title_show ──────────────────────────────────────────────────────── */
GameState title_show(void) {
  int blink = 0;
  int blink_frames = 0;

  /* decorative star positions (spread across full width) */
  static const int star_x[] = {5,   12,  22, 35, 48, 58, 68, 78, 88,  98,
                               108, 115, 10, 25, 40, 55, 70, 85, 100, 112};
  static const int star_y[] = {2, 5, 3, 6, 2, 7, 4, 2, 6, 3,
                               5, 7, 8, 4, 8, 3, 7, 5, 4, 2};
  int nstars = 20;

  while (1) {
    screen_clear();
    draw_border();

    /* ── decorative stars ── */
    int si;
    for (si = 0; si < nstars; si++) {
      screen_draw_char(star_x[si], star_y[si], '.');
    }

    /* ── ASCII art banner ── centred vertically in upper half ── */
    /* Banner is 6 rows tall + decorations = ~10 rows total.
       Place the top of the banner at ~1/3 from the top of play area. */
    int start_x = (SCREEN_W - 60) / 2;      /* centre 60-col banner */
    int start_y = (BORDER_BOTTOM - 10) / 3; /* ~1/3 from top        */

    int letter, row;
    for (letter = 0; letter < 6; letter++) {
      for (row = 0; row < 6; row++) {
        screen_draw_str(start_x + letter * 10, start_y + row,
                        banner[letter][row]);
      }
    }

    /* ── top decoration line ── */
    screen_draw_str(SCREEN_W / 2 - 20, start_y - 1,
                    "= = = = = = = = = = = = = = = = = = = =");

    /* ── subtitle ── */
    screen_draw_str(SCREEN_W / 2 - 19, start_y + 7,
                    "V E R T I C A L   S H O O T E R   *   2 0 2 6");

    /* ── bottom decoration line ── */
    screen_draw_str(SCREEN_W / 2 - 20, start_y + 8,
                    "= = = = = = = = = = = = = = = = = = = =");

    /* ── blinking prompt ── */
    if (blink) {
      screen_draw_str(SCREEN_W / 2 - 12, start_y + 11,
                      "~  PRESS ANY KEY TO CONTINUE  ~");
    }

    /* ── controls reminder ── */
    screen_draw_str(SCREEN_W / 2 - 18, BORDER_BOTTOM - 4,
                    "WASD / ARROWS: Move    SPACE: Shoot    Q: Quit");

    screen_flip();

    /* handle input */
    Key k = kb_get_key();
    if (k != KEY_NONE) {
      /* go to username if no name saved, else menu */
      User *u = user_get_current();
      if (u->name[0] == '\0')
        return STATE_USERNAME;
      return STATE_MENU;
    }

    /* blink every ~15 frames */
    blink_frames++;
    if (blink_frames >= 15) {
      blink = !blink;
      blink_frames = 0;
    }

    usleep(FRAME_US);
  }
}