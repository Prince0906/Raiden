#ifndef HUD_H
#define HUD_H

void hud_init(void);
void hud_update(void);                          /* survival score tick    */
void hud_add_score(int n);                      /* kill score from enemies */
void hud_draw(int health, int weapon_level);    /* renders HUD each frame */
int  hud_get_score(void);

#endif /* HUD_H */