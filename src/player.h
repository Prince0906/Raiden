#ifndef PLAYER_H
#define PLAYER_H

#include "keyboard.h"

/*
 * Player — the plane the user controls.
 *   x, y        : centre/nose position in screen-space
 *   health      : current HP (0..PLAYER_MAX_HEALTH)
 *   invincible  : countdown of immunity frames after taking a hit.
 *                 While > 0 the plane is drawn flashing and damage is ignored.
 */
typedef struct {
    int x;
    int y;
    int health;
    int invincible;
} Player;

Player *player_init(void);
void    player_move(Player *p, KeyState ks);  /* bitmask: supports diagonal */
void    player_update(Player *p);           /* tick invincibility timer      */
void    player_take_damage(Player *p, int dmg); /* apply dmg if not immune   */
int     player_is_dead(const Player *p);    /* returns 1 when health <= 0    */
void    player_draw(const Player *p);

#endif /* PLAYER_H */