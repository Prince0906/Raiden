#ifndef PLAYER_H
#define PLAYER_H

#include "keyboard.h"

typedef struct {
    int x;
    int y;
    int health;
    int invincible;
} Player;

Player *player_init(void);
void    player_move(Player *p, Key k);
void    player_update(Player *p);           
void    player_take_damage(Player *p, int dmg); 
int     player_is_dead(const Player *p);    
void    player_draw(const Player *p);

#endif 