#include "screen.h"
#include "math.h"
#include "memory.h"
#include "config.h"
#include <stdio.h>   

#define BUF_SIZE  (SCREEN_W * SCREEN_H)

static char *front = 0;
static char *back  = 0;

void screen_init(void) {
    front = (char *)my_alloc(BUF_SIZE);
    back  = (char *)my_alloc(BUF_SIZE);

    int i;
    for (i = 0; i < BUF_SIZE; i++) { front[i] = ' '; back[i] = ' '; }

    printf("\033[?25l");   
    printf("\033[2J");     
    printf("\033[H");      
    fflush(stdout);
}

void screen_clear(void) {
    int i;
    for (i = 0; i < BUF_SIZE; i++) back[i] = ' ';
}

void screen_draw_char(int x, int y, char c) {
    if (!in_bounds(x, 0, SCREEN_W - 1)) return;
    if (!in_bounds(y, 0, SCREEN_H - 1)) return;
    back[y * SCREEN_W + x] = c;
}

void screen_draw_str(int x, int y, const char *s) {
    int i = 0;
    while (s[i] != '\0') {
        screen_draw_char(x + i, y, s[i]);
        i++;
    }
}

void screen_flip(void) {
    int i;
    for (i = 0; i < BUF_SIZE; i++) {
        if (back[i] != front[i]) {
            int row = i / SCREEN_W + 1;   
            int col = i % SCREEN_W + 1;
            printf("\033[%d;%dH%c", row, col, back[i]);
            front[i] = back[i];
        }
    }
    fflush(stdout);
}

void screen_restore(void) {
    printf("\033[%d;1H", SCREEN_H + 1);   
    printf("\033[?25h");                   
    fflush(stdout);
}
