#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_W            80
#define SCREEN_H            24

#define BORDER_TOP           0
#define BORDER_BOTTOM       (SCREEN_H - 3)          
#define BORDER_LEFT          0
#define BORDER_RIGHT        (SCREEN_W - 1)           

#define PLAY_X_MIN          (BORDER_LEFT + 1)        
#define PLAY_X_MAX          (BORDER_RIGHT - 1)       
#define PLAY_Y_MIN          (BORDER_TOP + 1)         
#define PLAY_Y_MAX          (BORDER_BOTTOM - 1)      

#define PLAYER_START_X      (SCREEN_W / 2)
#define PLAYER_START_Y      (PLAY_Y_MAX - 3)         
#define PLANE_HALF_W         1                        
#define PLANE_HEIGHT         2                        

#define BORDER_HORIZ        '-'
#define BORDER_VERT         '|'
#define BORDER_CORNER       '+'

#define HUD_ROW             (SCREEN_H - 2)           
#define HUD_SCORE_COL        2

#define BUSY_WAIT_ITERS      2000000

#define MAX_BULLETS          5
#define BULLET_SPEED_FRAMES  80
#define BULLET_SPAWN_FRAMES  300
#define BULLET_GLYPH         '*'
#define BULLET_DMG_DEFAULT   20   

#define PLAYER_MAX_HEALTH        100
#define PLAYER_INVINCIBLE_FRAMES 200  

#define SCORE_INTERVAL       60

#define POOL_SIZE_BYTES     (64 * 1024)

#define INT_BUF_SIZE        12

#endif 