#include "keyboard.h"
#include <stdio.h>     
#include <stdlib.h>    
#include <termios.h>   

static struct termios orig_termios;
static int kb_initialized = 0;

void kb_restore(void) {
    if (kb_initialized) {
        tcsetattr(0, TCSANOW, &orig_termios);
        kb_initialized = 0;
    }
}

void kb_init(void) {
    tcgetattr(0, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= (unsigned int)~(ECHO | ICANON);  
    raw.c_cc[VMIN]  = 0;   
    raw.c_cc[VTIME] = 0;   

    tcsetattr(0, TCSANOW, &raw);
    setvbuf(stdin, NULL, _IONBF, 0);  
    kb_initialized = 1;

    atexit(kb_restore);   
}

Key kb_get_key(void) {
    clearerr(stdin);    
    int c = getchar();
    if (c == EOF) return KEY_NONE;

    switch (c) {
        case 'w': case 'W': return KEY_W;
        case 'a': case 'A': return KEY_A;
        case 's': case 'S': return KEY_S;
        case 'd': case 'D': return KEY_D;
        case ' ':           return KEY_SPACE;
        case 'q': case 'Q': return KEY_Q;
        case '\033': {
            int c2 = getchar();
            if (c2 == '[') {
                int c3 = getchar();
                switch (c3) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                    default:  return KEY_NONE;
                }
            }
            return KEY_NONE;
        }
        default: return KEY_NONE;
    }
}

Key kb_drain_key(void) {
    Key last = KEY_NONE;
    Key k;
    while ((k = kb_get_key()) != KEY_NONE) {
        last = k;
    }
    return last;
}