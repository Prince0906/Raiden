#include "leaderboard.h"
#include "config.h"
#include "keyboard.h"
#include "screen.h"
#include "string.h" /* my_strcpy, my_strcmp, my_strlen, int_to_str, str_to_int */
#include "user.h"
#include <fcntl.h>  /* open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC */
#include <unistd.h> /* read, write, close, usleep */

#define MAX_ENTRIES 10
#define PLAYER_NAME_MAX 50
#define FILE_BUF_SIZE 1024 /* leaderboard.txt will never exceed this */

/* ── ScoreEntry ──────────────────────────────────────────────────────── */
typedef struct {
  char name[PLAYER_NAME_MAX];
  int score;
} ScoreEntry;

static const char *LEADERBOARD_FILE = "leaderboard.txt";

/* ── file_read ───────────────────────────────────────────────────────── */
/* Reads entire file into buf[]. Returns bytes read, or 0 on error.       */
static int file_read(const char *path, char *buf, int buf_size) {
  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return 0;
  int n = (int)read(fd, buf, (unsigned int)(buf_size - 1));
  close(fd);
  if (n < 0)
    n = 0;
  buf[n] = '\0';
  return n;
}

/* ── parse_entries ───────────────────────────────────────────────────── */
/*
 * Parses text of the form "name score\n" into entries[].
 * Returns count of valid entries found (max MAX_ENTRIES).
 */
static int parse_entries(const char *buf, ScoreEntry *entries) {
  int count = 0;
  const char *p = buf;

  while (*p && count < MAX_ENTRIES) {
    /* read name token (up to first space or newline) */
    int ni = 0;
    while (*p && *p != ' ' && *p != '\n' && ni < PLAYER_NAME_MAX - 1) {
      entries[count].name[ni++] = *p++;
    }
    entries[count].name[ni] = '\0';

    if (ni == 0) { /* blank line — skip */
      while (*p && *p != '\n')
        p++;
      if (*p == '\n')
        p++;
      continue;
    }

    /* skip space(s) */
    while (*p == ' ')
      p++;

    /* read score digits */
    int score = 0;
    int got_digit = 0;
    while (*p >= '0' && *p <= '9') {
      score = score * 10 + (*p - '0');
      p++;
      got_digit = 1;
    }
    if (!got_digit) {
      /* corrupt line — skip to next */
      while (*p && *p != '\n')
        p++;
      if (*p == '\n')
        p++;
      continue;
    }

    entries[count].score = score;
    count++;

    /* skip to next line */
    while (*p && *p != '\n')
      p++;
    if (*p == '\n')
      p++;
  }
  return count;
}

/* ── write_entries ───────────────────────────────────────────────────── */
/* Serialises entries[] back to LEADERBOARD_FILE. */
static void write_entries(const ScoreEntry *entries, int count) {
  char out[FILE_BUF_SIZE];
  int pos = 0;
  int i;

  for (i = 0; i < count && pos < FILE_BUF_SIZE - 20; i++) {
    /* name */
    int ni = 0;
    while (entries[i].name[ni] && pos < FILE_BUF_SIZE - 15)
      out[pos++] = entries[i].name[ni++];
    out[pos++] = ' ';

    /* score as string */
    char sbuf[INT_BUF_SIZE];
    int_to_str(entries[i].score, sbuf, INT_BUF_SIZE);
    int si = 0;
    while (sbuf[si] && pos < FILE_BUF_SIZE - 5)
      out[pos++] = sbuf[si++];
    out[pos++] = '\n';
  }

  int fd = open(LEADERBOARD_FILE, O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0644);
  if (fd >= 0) {
    write(fd, out, (unsigned int)pos);
    close(fd);
  }
}

/* ── leaderboard_save ────────────────────────────────────────────────── */
void leaderboard_save(const char *name, int score) {
  char buf[FILE_BUF_SIZE];
  ScoreEntry entries[MAX_ENTRIES + 1]; /* +1 for the new entry */
  int count;

  file_read(LEADERBOARD_FILE, buf, FILE_BUF_SIZE);
  count = parse_entries(buf, entries);

  /* Append new entry */
  my_strcpy(entries[count].name, name);
  entries[count].score = score;
  count++;

  /* Sort descending (insertion sort — stable, simple) */
  int i, j;
  for (i = 1; i < count; i++) {
    ScoreEntry tmp = entries[i];
    j = i - 1;
    while (j >= 0 && entries[j].score < tmp.score) {
      entries[j + 1] = entries[j];
      j--;
    }
    entries[j + 1] = tmp;
  }

  /* Keep only top MAX_ENTRIES */
  if (count > MAX_ENTRIES)
    count = MAX_ENTRIES;

  write_entries(entries, count);
}

/* ── leaderboard_get_rank ────────────────────────────────────────────── */
int leaderboard_get_rank(int score) {
  char buf[FILE_BUF_SIZE];
  ScoreEntry entries[MAX_ENTRIES];
  int count;

  file_read(LEADERBOARD_FILE, buf, FILE_BUF_SIZE);
  count = parse_entries(buf, entries);

  /* Count how many existing entries beat this score */
  int rank = 1;
  int i;
  for (i = 0; i < count; i++) {
    if (entries[i].score > score)
      rank++;
  }
  if (rank > MAX_ENTRIES)
    return 0; /* not in top 10 */
  return rank;
}

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

/* ── leaderboard_show ────────────────────────────────────────────────── */
GameState leaderboard_show(void) {
  char buf[FILE_BUF_SIZE];
  ScoreEntry entries[MAX_ENTRIES];
  int count;

  file_read(LEADERBOARD_FILE, buf, FILE_BUF_SIZE);
  count = parse_entries(buf, entries);

  const char *current_name = user_get_current()->name;
  int mid_x = SCREEN_W / 2;
  int mid_y = (BORDER_TOP + BORDER_BOTTOM) / 2;
  /* leaderboard block: header at mid_y-8, up to 10 entries×2 rows below */
  int header_y = mid_y - 9;

  while (1) {
    screen_clear();
    draw_border();

    /* ── header ── */
    screen_draw_str(mid_x - 10, header_y, "== TOP RAIDERS ==");
    screen_draw_str(mid_x - 14, header_y + 1,
                    "- - - - - - - - - - - - - - - - -");

    if (count == 0) {
      screen_draw_str(mid_x - 13, header_y + 3, "No scores yet. Play first!");
    } else {
      int i;
      for (i = 0; i < count; i++) {
        int row = header_y + 3 + i * 2;

        /* ── rank number "#N  " ── */
        char rank_buf[4] = {'#', (char)('1' + i), ' ', '\0'};
        if (i >= 9)
          rank_buf[1] = (char)('0' + (i + 1)); /* handle #10 simply */
        screen_draw_str(mid_x - 20, row, rank_buf);

        /* ── name ── */
        screen_draw_str(mid_x - 16, row, entries[i].name);

        /* ── score (right-aligned at mid_x+12) ── */
        char sbuf[INT_BUF_SIZE];
        int_to_str(entries[i].score, sbuf, INT_BUF_SIZE);
        int score_col = mid_x + 12 - my_strlen(sbuf);
        screen_draw_str(score_col, row, sbuf);

        /* ── highlight current player ── */
        if (my_strcmp(entries[i].name, current_name) == 0) {
          screen_draw_str(mid_x + 14, row, "<--  YOU");
        }
      }
    }

    /* ── footer ── */
    screen_draw_str(mid_x - 14, BORDER_BOTTOM - 2,
                    "[ ENTER ]  or  [ Q ]  Back to Menu");

    screen_flip();

    /* poll input */
    usleep(FRAME_US);
    Key k = kb_get_key();
    if (k == KEY_ENTER || k == KEY_Q)
      return STATE_MENU;
  }
}