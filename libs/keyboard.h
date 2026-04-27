#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef int Key;

#define KEY_NONE 0
#define KEY_W (-1)
#define KEY_A (-2)
#define KEY_S (-3)
#define KEY_D (-4)
#define KEY_UP (-5)
#define KEY_DOWN (-6)
#define KEY_LEFT (-7)
#define KEY_RIGHT (-8)
#define KEY_SPACE (-9)
#define KEY_Q (-10)
#define KEY_ENTER (-11)
#define KEY_BACKSPACE (-12)

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
char*    kb_read_string(int max_len);

#endif /* KEYBOARD_H */
