#ifndef GAMESTATE_H
#define GAMESTATE_H

/*
 * GameState — the central state machine enum.
 * Every screen/handler returns one of these to signal main() what comes next.
 */
typedef enum {
  STATE_TITLE,       /* big ASCII-art welcome screen             */
  STATE_USERNAME,    /* type your username                       */
  STATE_MENU,        /* main menu: Start / Leaderboard / Name    */
  STATE_PLAYING,     /* the active game loop                     */
  STATE_GAMEOVER,    /* game over panel with score + rank        */
  STATE_LEADERBOARD, /* top-10 leaderboard                       */
  STATE_QUIT         /* clean exit                               */
} GameState;

#endif /* GAMESTATE_H */