#include <stdlib.h>   /* exit() — explicitly allowed */
#include <unistd.h>   /* usleep — POSIX timing       */

#include "gamestate.h"
#include "config.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "string.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "hud.h"
#include "renderer.h"
#include "user.h"
#include "title.h"
#include "menu.h"
#include "gameover.h"
#include "leaderboard.h"

/* ── username_prompt ─────────────────────────────────────────────────── */
/*
 * Lets the player type a username (up to 49 chars).
 * Draws everything inside the boundary box.
 * Loops back if the name is empty or taken.
 * Returns STATE_MENU when a valid unique name is accepted.
 */
static void draw_border(void) {
    int i;
    for (i = BORDER_LEFT; i <= BORDER_RIGHT; i++) {
        screen_draw_char(i, BORDER_TOP,    BORDER_HORIZ);
        screen_draw_char(i, BORDER_BOTTOM, BORDER_HORIZ);
    }
    for (i = BORDER_TOP; i <= BORDER_BOTTOM; i++) {
        screen_draw_char(BORDER_LEFT,  i, BORDER_VERT);
        screen_draw_char(BORDER_RIGHT, i, BORDER_VERT);
    }
    screen_draw_char(BORDER_LEFT,  BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_TOP,    BORDER_CORNER);
    screen_draw_char(BORDER_LEFT,  BORDER_BOTTOM, BORDER_CORNER);
    screen_draw_char(BORDER_RIGHT, BORDER_BOTTOM, BORDER_CORNER);
}

static GameState username_prompt(void) {
    char name[50];
    int  len = 0;
    int  show_taken = 0;
    int  taken_timer = 0;

    /* clear name buffer */
    int ci;
    for (ci = 0; ci < 50; ci++) name[ci] = 0;

    while (1) {
        screen_clear();
        draw_border();

        int mid_x = SCREEN_W / 2;
        int mid_y = SCREEN_H / 2;

        screen_draw_str(mid_x - 8,  mid_y - 4, "NEW PILOT");
        screen_draw_str(mid_x - 10, mid_y - 3, "- - - - - - - - - -");

        screen_draw_str(mid_x - 14, mid_y - 1, "Enter your callsign:");
        screen_draw_str(mid_x - 14, mid_y,     name);

        /* cursor blink — always show underscore at end */
        screen_draw_char(mid_x - 14 + len, mid_y, '_');

        /* format rule */
        screen_draw_str(mid_x - 17, mid_y + 2,
                        "(lowercase letters and digits only, no spaces)");

        if (show_taken) {
            screen_draw_str(mid_x - 13, mid_y + 4,
                            "! Callsign already taken !");
        }

        screen_draw_str(mid_x - 16, BORDER_BOTTOM - 2,
                        "Type your callsign then press ENTER");

        screen_flip();

        /* decay taken-notice */
        if (show_taken) {
            taken_timer++;
            if (taken_timer > 40) { show_taken = 0; taken_timer = 0; }
        }

        usleep(10000);
        Key k = kb_get_key();

        if (k == KEY_ENTER) {
            if (len == 0) continue;          /* need at least 1 char */
            if (user_exists(name)) {
                show_taken = 1;
                taken_timer = 0;
                continue;
            }
            user_save(name);
            return STATE_MENU;
        } else if (k == KEY_BACKSPACE && len > 0) {
            name[--len] = 0;
        } else if (len < 49) {
            /* auto-lowercase uppercase letters */
            char ch = (char)k;
            if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + ('a' - 'A'));

            /* accept only a-z and 0-9 — silently drop everything else */
            if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
                name[len++] = ch;
                name[len]   = 0;
            }
        }
    }
}

/* ── run_game_loop ───────────────────────────────────────────────────── */
/*
 * The actual game.  Extracted from main() so it can be called multiple
 * times (replay).
 *
 * Returns:
 *   STATE_GAMEOVER  — player died
 *   STATE_MENU      — player pressed Q (quit to menu)
 */
static GameState run_game_loop(void) {
    /* Re-initialise game entities for a fresh round */
    Player *player = player_init();
    bullets_init();
    enemies_init();
    hud_init();

    unsigned int frame = 0;

    while (!player_is_dead(player)) {
        KeyState ks = kb_drain_keys();
        if (ks & KS_QUIT) {
            return STATE_MENU;    /* Q → back to menu, not full quit */
        }

        /* a. Input */
        if (ks & KS_SPACE) player_shoot(player);

        /* b. Update */
        player_move(player, ks);
        player_update(player);
        bullets_update(frame);
        enemies_update(frame);

        /* c. Player bullets vs enemies */
        {
            int kill_score = enemies_process_player_bullets();
            if (kill_score > 0) hud_add_score(kill_score);
        }

        /* d. Collision */
        {
            int dmg = bullets_check_hit(player->x, player->y);
            dmg    += enemies_check_hit(player->x, player->y);
            if (dmg > 0) player_take_damage(player, dmg);
        }

        /* e. Survival score */
        if (frame % SCORE_INTERVAL == 0) hud_update();

        /* f. Render */
        renderer_draw_frame(player);

        usleep(FRAME_US);
        frame++;
    }

    return STATE_GAMEOVER;
}

/* ── main ────────────────────────────────────────────────────────────── */
int main(void) {
    /* 1. Initialise subsystems — order matters: memory first */
    mem_init();
    kb_init();
    screen_init();
    user_init();
    user_load_current();

    /* 2. State machine — everything lives in here */
    GameState state = STATE_TITLE;
    int last_score  = 0;
    int last_rank   = 0;

    while (state != STATE_QUIT) {
        switch (state) {

            case STATE_TITLE:
                state = title_show();
                break;

            case STATE_USERNAME:
                state = username_prompt();
                break;

            case STATE_MENU:
                state = menu_show();
                break;

            case STATE_PLAYING:
                state = run_game_loop();
                if (state == STATE_GAMEOVER) {
                    /* save score and compute rank before showing game over */
                    last_score = hud_get_score();
                    leaderboard_save(user_get_current()->name, last_score);
                    last_rank  = leaderboard_get_rank(last_score);
                }
                break;

            case STATE_GAMEOVER:
                state = gameover_show(last_score, last_rank);
                break;

            case STATE_LEADERBOARD:
                state = leaderboard_show();
                break;

            default:
                state = STATE_QUIT;
                break;
        }
    }

    /* 3. Cleanup — kb_restore runs via atexit registered in kb_init */
    screen_restore();
    return 0;
}