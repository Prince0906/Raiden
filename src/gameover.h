#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "gamestate.h"

/*
 * gameover_show — renders the Game Over panel with final score and rank.
 *
 * score : the player's final score
 * rank  : 1-based rank in the leaderboard (0 = unranked / off-board)
 *
 * Returns:
 *   STATE_MENU        — player pressed R (replay / back to menu)
 *   STATE_LEADERBOARD — player pressed L (view leaderboard)
 *   STATE_QUIT        — player pressed Q
 */
GameState gameover_show(int score, int rank);

#endif /* GAMEOVER_H */