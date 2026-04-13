#ifndef LIB_SCREEN_H
#define LIB_SCREEN_H

void screen_init(void);
void screen_clear(void);
void screen_draw_char(int x, int y, char c);
void screen_draw_str(int x, int y, const char *s);
void screen_flip(void);
void screen_restore(void);

#endif 
