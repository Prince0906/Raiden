#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
    KEY_NONE = 0,
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SPACE, KEY_Q
} Key;

void kb_init(void);
void kb_restore(void);
Key  kb_get_key(void);
Key  kb_drain_key(void);  

#endif 
