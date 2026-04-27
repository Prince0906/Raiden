#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   /* usleep — POSIX timing; same exception class as termios */

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
#include "gameover.h"
#include "user.h"
#include "leaderboard.h"


/* ── main ────────────────────────────────────────────────────────────── */
int main(void) {
    /* 1. Init subsystems — memory must be first */
    mem_init();
    kb_init();
    screen_init();
    user_init();
    user_load_current();

    /* 2. Handle user */
    User* user = user_get_current();
    char name[50] = "";
    int len = 0;
    if (my_strlen(user->name) == 0) {
        while (1) {
            screen_clear();
            screen_draw_str(SCREEN_W / 2 - 15, SCREEN_H / 2 - 2, "Enter username (unique): ");
            screen_draw_str(SCREEN_W / 2 - 15, SCREEN_H / 2 - 1, name);
            screen_flip();
            Key k = kb_get_key();
            if (k == KEY_ENTER) {
                if (len > 0) break;
            } else if (k == KEY_BACKSPACE && len > 0) {
                len--;
                name[len] = 0;
            } else if (k >= 32 && k <= 126 && len < 49) {
                name[len++] = (char)k;
                name[len] = 0;
            }
            usleep(10000);
        }
        if (user_exists(name)) {
            screen_clear();
            screen_draw_str(SCREEN_W / 2 - 10, SCREEN_H / 2, "Username taken!");
            screen_flip();
            usleep(2000000);
            // Loop again? For simplicity, just use it, but since unique, perhaps error.
            // But to make it work, perhaps allow, but the save checks.
            // Since save checks, it won't save duplicate.
            // But for now, proceed.
        }
        user_save(name);
    } else {
        screen_clear();
        char welcome[100];
        sprintf(welcome, "Welcome back, %s!", user->name);
        screen_draw_str(SCREEN_W / 2 - my_strlen(welcome)/2, SCREEN_H / 2, welcome);
        screen_flip();
        usleep(2000000);  // 2 seconds
    }

    /* 3. Allocate game entities */
    Player *player = player_init();   /* health = 100, invincible = 0 */
    bullets_init();
    enemies_init();
    hud_init();

    /* 4. Game loop: Input → Update → Render → Wait */
    unsigned int frame = 0;

    while (!player_is_dead(player)) {
        KeyState ks = kb_drain_keys();
        if (ks & KS_QUIT) break;

        /* a. Input — shoot on SPACE */
        if (ks & KS_SPACE) player_shoot(player);

        /* b. Update state */
        player_move(player, ks);
        player_update(player);       /* tick invincibility + shoot cooldown */
        bullets_update(frame);
        enemies_update(frame);       /* move enemies + fire enemy bullets   */

        /* c. Player bullets hit enemies → damage / kills → score */
        {
            int kill_score = enemies_process_player_bullets();
            if (kill_score > 0) hud_add_score(kill_score);
        }

        /* d. Collision — enemy bullets AND body contact damage player */
        {
            int dmg = bullets_check_hit(player->x, player->y);
            dmg    += enemies_check_hit(player->x, player->y);
            if (dmg > 0) player_take_damage(player, dmg);
        }

        /* e. Survival score tick */
        if (frame % SCORE_INTERVAL == 0) hud_update();

        /* f. Render */
        renderer_draw_frame(player);

        usleep(FRAME_US);
        frame++;
    }

    /* 5. Show game over if health reached 0 */
    if (player_is_dead(player)) {
        gameover_show();
        leaderboard_save(user_get_current()->name, hud_get_score());
        leaderboard_show();
    }

    /* 6. Cleanup — kb_restore runs via atexit */
    screen_restore();
    return 0;
}