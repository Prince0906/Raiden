#ifndef MENU_H
#define MENU_H

#include "gamestate.h"

/*
 * menu_show — renders the navigable main menu (Start / Leaderboard /
 * Change Name / Quit).  W/S or arrows move the cursor; ENTER selects.
 * Returns the GameState corresponding to the selection.
 */
GameState menu_show(void);

#endif /* MENU_H */