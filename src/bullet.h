#ifndef BULLET_H
#define BULLET_H

/*
 * Bullet — a single falling projectile.
 *   x, y    : current position in screen-space
 *   active  : 0 = slot is free, 1 = bullet is on screen
 *   damage  : health points removed when this bullet hits the player
 *             (prepared for future bullet types with different damage values)
 */
typedef struct {
    int x;
    int y;
    int active;
    int damage;
} Bullet;

void bullets_init(void);
void bullets_update(unsigned int frame);
void bullets_draw(void);

/*
 * bullets_check_hit — checks if any active bullet overlaps the player plane.
 *   Returns : damage dealt (> 0 means a hit occurred)
 *             0 means no hit this frame
 *   Side effect: the bullet that caused the hit is deactivated (consumed),
 *                so one bullet cannot deal damage across multiple frames.
 */
int bullets_check_hit(int px, int py);

#endif /* BULLET_H */