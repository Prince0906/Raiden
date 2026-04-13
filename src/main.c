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
    Player *player = player_init();   /* health = 100, invincible = 0 */
    bullets_init();
    hud_init();

    /* 3. Game loop: Input → Update → Render → Wait */
    unsigned int frame = 0;

    while (!player_is_dead(player)) {
        Key k = kb_drain_key();
        if (k == KEY_Q) break;

        /* a. Update state */
        player_move(player, k);
        player_update(player);       /* tick invincibility countdown */
        bullets_update(frame);



        /* c. Score ticks up just for surviving */
        if (frame % SCORE_INTERVAL == 0) {
            hud_update();
        }

        /* d. Render */
        renderer_draw_frame(player);

        busy_wait();
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