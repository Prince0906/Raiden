#ifndef BULLET_H
#define BULLET_H

typedef struct {
    int x;
    int y;
    int active;
    int damage;
} Bullet;

void bullets_init(void);
void bullets_update(unsigned int frame);
void bullets_draw(void);

int bullets_check_hit(int px, int py);

#endif 