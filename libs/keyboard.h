#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
    KEY_NONE = 0,
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SPACE, KEY_Q
} Key;

/*
 * KeyState — bitmask of all directions/actions active THIS frame.
 * Multiple bits can be set simultaneously, enabling diagonal movement.
 */
typedef unsigned int KeyState;

#define KS_UP     (1u << 0)
#define KS_DOWN   (1u << 1)
#define KS_LEFT   (1u << 2)
#define KS_RIGHT  (1u << 3)
#define KS_SPACE  (1u << 4)
#define KS_QUIT   (1u << 5)

void     kb_init(void);
void     kb_restore(void);
Key      kb_get_key(void);
Key      kb_drain_key(void);    /* legacy: drains buffer, returns last key  */
KeyState kb_drain_keys(void);   /* V4: drains buffer, returns direction bitmask */

#endif /* KEYBOARD_H */
