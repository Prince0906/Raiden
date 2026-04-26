#ifndef PLAYER_H
#define PLAYER_H

#include "keyboard.h"

/*
 * Player — the plane the user controls.
 *   x, y          : centre/nose position in screen-space
 *   health        : current HP (0..PLAYER_MAX_HEALTH)
 *   invincible    : immunity frames countdown after taking a hit
 *   weapon_level  : 0=single  1=boosted  2=triple  3=wide spread
 *   shoot_cooldown: frames until next SPACE shot is allowed
 */
typedef struct {
    int x;
    int y;
    int health;
    int invincible;
    int weapon_level;
    int shoot_cooldown;
} Player;

Player *player_init(void);
void    player_move(Player *p, KeyState ks);
void    player_update(Player *p);           /* tick timers each frame        */
void    player_shoot(Player *p);            /* fire bullets upward (SPACE)   */
void    player_take_damage(Player *p, int dmg);
int     player_is_dead(const Player *p);
void    player_draw(const Player *p);

#endif /* PLAYER_H */