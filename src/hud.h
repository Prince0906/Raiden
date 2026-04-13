#ifndef HUD_H
#define HUD_H

void hud_init(void);
void hud_update(void);
void hud_draw(int health);   /* renders SCORE + health bar each frame */
int  hud_get_score(void);

#endif /* HUD_H */