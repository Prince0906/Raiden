#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   /* usleep — POSIX timing; same exception class as termios */

#include "config.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "hud.h"
#include "renderer.h"
#include "gameover.h"


/* ── main ────────────────────────────────────────────────────────────── */
int main(void) {
    /* 1. Init subsystems — memory must be first */
    mem_init();
    kb_init();
    screen_init();

    /* 2. Allocate game entities */
    Player *player = player_init();   /* health = 100, invincible = 0 */
    bullets_init();
    enemies_init();
    hud_init();

    /* 3. Game loop: Input → Update → Render → Wait */
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

    /* 4. Show game over if health reached 0 */
    if (player_is_dead(player)) {
        gameover_show();
    }

    /* 5. Cleanup — kb_restore runs via atexit */
    screen_restore();
    return 0;
}