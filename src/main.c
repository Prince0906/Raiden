#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
#include "gamestate.h"
#include "title.h"
#include "menu.h"

int main(void) {
    /* 1. Init subsystems */
    mem_init();
    kb_init();
    screen_init();
    user_init();
    user_load_current();

    GameState state = STATE_TITLE;
    
    while (state != STATE_QUIT) {
        switch (state) {
            case STATE_TITLE:
                state = title_show();
                break;
                
            case STATE_USERNAME: {
                char name[50] = "";
                int len = 0;
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
                user_save(name);
                state = STATE_MENU;
                break;
            }
                
            case STATE_MENU:
                state = menu_show();
                break;
                
            case STATE_PLAYING: {
                /* Allocate game entities */
                Player *player = player_init();   /* health = 100, invincible = 0 */
                bullets_init();
                enemies_init();
                hud_init();

                unsigned int frame = 0;
                int quit_early = 0;

                while (!player_is_dead(player)) {
                    KeyState ks = kb_drain_keys();
                    if (ks & KS_QUIT) {
                        quit_early = 1;
                        break;
                    }

                    if (ks & KS_SPACE) player_shoot(player);

                    player_move(player, ks);
                    player_update(player);
                    bullets_update(frame);
                    enemies_update(frame);

                    {
                        int kill_score = enemies_process_player_bullets();
                        if (kill_score > 0) hud_add_score(kill_score);
                    }

                    {
                        int dmg = bullets_check_hit(player->x, player->y);
                        dmg    += enemies_check_hit(player->x, player->y);
                        if (dmg > 0) player_take_damage(player, dmg);
                    }

                    if (frame % SCORE_INTERVAL == 0) hud_update();

                    renderer_draw_frame(player);

                    usleep(FRAME_US);
                    frame++;
                }
                
                if (quit_early) {
                    state = STATE_MENU;
                } else {
                    state = STATE_GAMEOVER;
                }
                break;
            }
                
            case STATE_GAMEOVER: {
                int final_score = hud_get_score();
                leaderboard_save(user_get_current()->name, final_score);
                int rank = leaderboard_get_rank(final_score);
                state = gameover_show(final_score, rank);
                break;
            }
                
            case STATE_LEADERBOARD:
                state = leaderboard_show();
                break;
                
            default:
                state = STATE_QUIT;
                break;
        }
    }

    /* Cleanup */
    screen_restore();
    return 0;
}