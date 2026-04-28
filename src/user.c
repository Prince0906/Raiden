#include "user.h"
#include "string.h" /* my_strcpy, my_strcmp, my_strlen */
#include <fcntl.h>  /* open, O_RDONLY, O_WRONLY, O_CREAT, O_APPEND */
#include <unistd.h> /* read, write, close */

#define USER_FILE "users.txt"
#define CURRENT_USER_FILE "current_user.txt"
#define USER_BUF_SIZE 2048

static User current_user;

/* ── user_init ───────────────────────────────────────────────────────── */
void user_init(void) {
  int i;
  for (i = 0; i < 50; i++)
    current_user.name[i] = 0;
}

/* ── file_read_line ──────────────────────────────────────────────────── */
/*
 * Reads the first line of 'path' (up to max_len-1 chars) into 'out'.
 * Strips the trailing newline. Returns 1 on success, 0 on error.
 */
static int file_read_first_line(const char *path, char *out, int max_len) {
  char buf[USER_BUF_SIZE];
  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return 0;
  int n = (int)read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (n <= 0)
    return 0;
  buf[n] = '\0';

  /* copy up to newline */
  int i = 0;
  while (i < n && buf[i] != '\n' && i < max_len - 1) {
    out[i] = buf[i];
    i++;
  }
  out[i] = '\0';
  return (i > 0) ? 1 : 0;
}

/* ── user_exists ─────────────────────────────────────────────────────── */
/*
 * Reads users.txt line by line. Each line is a username.
 * Compares each line against 'name' using our custom my_strcmp.
 */
int user_exists(const char *name) {
  char buf[USER_BUF_SIZE];
  int fd = open(USER_FILE, O_RDONLY);
  if (fd < 0)
    return 0;
  int n = (int)read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (n <= 0)
    return 0;
  buf[n] = '\0';

  /* walk line by line */
  char line[50];
  int li = 0;
  int bi;
  for (bi = 0; bi <= n; bi++) {
    char c = (bi < n) ? buf[bi] : '\n'; /* force final newline */
    if (c == '\n') {
      line[li] = '\0';
      if (li > 0 && my_strcmp(line, name) == 0)
        return 1;
      li = 0;
    } else if (li < 49) {
      line[li++] = c;
    }
  }
  return 0;
}

/* ── user_save ───────────────────────────────────────────────────────── */
void user_save(const char *name) {
  /* append to users.txt if not already there */
  if (!user_exists(name)) {
    int fd = open(USER_FILE, O_WRONLY | O_CREAT | O_APPEND, (mode_t)0644);
    if (fd >= 0) {
      write(fd, name, (unsigned int)my_strlen(name));
      write(fd, "\n", 1);
      close(fd);
    }
  }

  /* overwrite current_user.txt */
  int fd = open(CURRENT_USER_FILE, O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0644);
  if (fd >= 0) {
    write(fd, name, (unsigned int)my_strlen(name));
    write(fd, "\n", 1);
    close(fd);
  }

  my_strcpy(current_user.name, name);
}

/* ── user_load_current ───────────────────────────────────────────────── */
void user_load_current(void) {
  file_read_first_line(CURRENT_USER_FILE, current_user.name, 50);
}

/* ── user_get_current ────────────────────────────────────────────────── */
User *user_get_current(void) { return &current_user; }