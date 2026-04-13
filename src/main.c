#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "player.h"
#include "bullet.h"
#include "hud.h"
#include "renderer.h"
#include "gameover.h"

static void busy_wait(void) {
    volatile int i;
    for (i = 0; i < BUSY_WAIT_ITERS; i++) {  }
}

int main(void) {

    mem_init();
    kb_init();
    screen_init();

    Player *player = player_init();   
    bullets_init();
    hud_init();

    unsigned int frame = 0;

    while (!player_is_dead(player)) {
        Key k = kb_drain_key();
        if (k == KEY_Q) break;

        player_move(player, k);
        player_update(player);       
        bullets_update(frame);

        {
            int dmg = bullets_check_hit(player->x, player->y);
            if (dmg > 0) {
                player_take_damage(player, dmg);
            }
        }

        if (frame % SCORE_INTERVAL == 0) {
            hud_update();
        }

        renderer_draw_frame(player);

        busy_wait();
        frame++;
    }

    if (player_is_dead(player)) {
        gameover_show();
    }

    screen_restore();
    return 0;
}