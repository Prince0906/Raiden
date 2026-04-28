#ifndef TITLE_H
#define TITLE_H

#include "gamestate.h"

/*
 * title_show — renders the ASCII-art RAIDEN banner and waits for any key.
 * Returns STATE_USERNAME if no user is saved yet, STATE_MENU otherwise.
 */
GameState title_show(void);

#endif /* TITLE_H */