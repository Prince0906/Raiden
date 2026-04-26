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
    int dx;    /* horizontal velocity: -1, 0, or +1 per move tick      */
    int dy;    /* vertical velocity:   +1 down (enemy), -1 up (player) */
} Bullet;

void bullets_init(void);

/*
 * bullet_spawn — public API for firing bullets from any source.
 *   x, y    : starting position (typically just below the shooter)
 *   dx, dy  : velocity per move tick (+1/-1/0)
 *   damage  : HP removed on player hit
 * Returns 1 if a free slot was found, 0 if pool is full (spawn skipped).
 */
int  bullet_spawn(int x, int y, int dx, int dy, int damage);
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