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

/* ── busy_wait ───────────────────────────────────────────────────────── */
static void busy_wait(void) {
    volatile int i;
    for (i = 0; i < BUSY_WAIT_ITERS; i++) { /* spin */ }
}

/* ── main ────────────────────────────────────────────────────────────── */
int main(void) {
    /* 1. Init subsystems — memory must be first */
    mem_init();
    kb_init();
    screen_init();

    /* 2. Allocate game entities */
    Player *player = player_init();
    bullets_init();
    hud_init();

    /* 3. Game loop: Input → Update → Render → Wait */
    unsigned int frame = 0;
    int          hit   = 0;

    while (!hit) {
        Key k = kb_get_key();
        if (k == KEY_Q) break;

        player_move(player, k);
        bullets_update(frame);

        if (bullets_hit(player->x, player->y)) {
            hit = 1;
            break;
        }

        if (frame % SCORE_INTERVAL == 0) {
            hud_update();
        }

        renderer_draw_frame(player);

        busy_wait();
        frame++;
    }

    /* 4. Hand off to the game over component if killed by a bullet */
    if (hit) {
        gameover_show();
    }

    /* 5. Cleanup */
    screen_restore();
    return 0;
}