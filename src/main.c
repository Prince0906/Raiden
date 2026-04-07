#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "string.h"
#include "player.h"
#include "hud.h"
#include "renderer.h"

/* ── busy_wait ───────────────────────────────────────────────────────── */
static void busy_wait(void) {
    volatile int i;
    for (i = 0; i < BUSY_WAIT_ITERS; i++) { /* spin */ }
}

/* ── main ────────────────────────────────────────────────────────────── */
int main(void) {
    /* 1. Init subsystems */
    mem_init();
    kb_init();
    screen_init();

    /* 2. Allocate game entities */
    Player *player = player_init();
    hud_init();

    /* 3. Game loop: Input -> Update -> Render -> Wait */
    while (1) {
        Key k = kb_get_key();
        if (k == KEY_Q) break;

        player_move(player, k);
        hud_update();

        renderer_draw_frame(player);

        busy_wait();
    }

    /* 4. Cleanup */
    screen_restore();
    return 0;
}
