#include "leaderboard.h"
#include "screen.h"
#include "keyboard.h"
#include "config.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ENTRIES 10

typedef struct {
    char name[50];
    int score;
} ScoreEntry;

static const char* LEADERBOARD_FILE = "leaderboard.txt";

void leaderboard_save(const char* name, int score) {
    ScoreEntry entries[MAX_ENTRIES + 1]; // +1 for new
    int count = 0;
    FILE* f = fopen(LEADERBOARD_FILE, "r");
    if (f) {
        while (count < MAX_ENTRIES && fscanf(f, "%49s %d", entries[count].name, &entries[count].score) == 2) {
            count++;
        }
        fclose(f);
    }
    // Add new
    my_strcpy(entries[count].name, name);
    entries[count].score = score;
    count++;
    // Sort descending
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (entries[i].score < entries[j].score) {
                ScoreEntry temp = entries[i];
                entries[i] = entries[j];
                entries[j] = temp;
            }
        }
    }
    // Save top 10
    f = fopen(LEADERBOARD_FILE, "w");
    if (f) {
        for (int i = 0; i < count && i < MAX_ENTRIES; i++) {
            fprintf(f, "%s %d\n", entries[i].name, entries[i].score);
        }
        fclose(f);
    }
}

void leaderboard_show(void) {
    ScoreEntry entries[MAX_ENTRIES];
    int count = 0;
    FILE* f = fopen(LEADERBOARD_FILE, "r");
    if (f) {
        while (count < MAX_ENTRIES && fscanf(f, "%49s %d", entries[count].name, &entries[count].score) == 2) {
            count++;
        }
        fclose(f);
    }
    screen_clear();
    screen_draw_str(SCREEN_W / 2 - 10, 5, "LEADERBOARD");
    for (int i = 0; i < count; i++) {
        char buf[100];
        sprintf(buf, "%d. %s - %d", i + 1, entries[i].name, entries[i].score);
        screen_draw_str(SCREEN_W / 2 - 15, 7 + i, buf);
    }
    screen_draw_str(SCREEN_W / 2 - 10, 7 + count + 2, "Press Q to exit");
    screen_flip();
    Key k = KEY_NONE;
    while (k != KEY_Q) {
        usleep(FRAME_US);
        k = kb_get_key();
    }
}