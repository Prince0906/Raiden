#ifndef PLAYER_H
#define PLAYER_H

#include "keyboard.h"

typedef struct {
    int x;       /* column of the plane's centre (nose tip)  */
    int y;       /* row of the plane's top (nose tip)        */
} Player;

Player *player_init(void);
void    player_move(Player *p, Key k);
void    player_draw(const Player *p);

#endif /* PLAYER_H */
