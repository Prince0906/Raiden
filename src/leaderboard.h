#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "gamestate.h"

/*
 * leaderboard_save — inserts name+score into leaderboard.txt, keeps top 10
 *                    sorted descending by score.
 */
void leaderboard_save(const char *name, int score);

/*
 * leaderboard_get_rank — returns the 1-based rank that 'score' would achieve
 *                        in the current leaderboard (1 = highest).
 *                        Returns 0 if the score would not make the top 10.
 */
int leaderboard_get_rank(int score);

/*
 * leaderboard_show — renders the top-10 leaderboard, highlights the
 *                    current player's entry.
 * Returns STATE_MENU when the player presses ENTER or Q.
 */
GameState leaderboard_show(void);

#endif /* LEADERBOARD_H */