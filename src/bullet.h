#ifndef BULLET_H
#define BULLET_H

/*
 * Bullet — a single falling projectile.
 *   x, y    : current position in screen-space
 *   active  : 0 = slot is free, 1 = bullet is on screen
 */
typedef struct {
    int x;
    int y;
    int active;
} Bullet;

void bullets_init(void);
void bullets_update(unsigned int frame);
void bullets_draw(void);

/*
 * bullets_hit — returns 1 if any active bullet overlaps
 * any of the 4 cells occupied by the player plane.
 * px, py = plane's centre/nose position.
 */
int  bullets_hit(int px, int py);

#endif /* BULLET_H */