#ifndef BULLET_H
#define BULLET_H

/*
 * Bullet — a projectile fired by an enemy or the player.
 *   x, y      : current screen position
 *   active    : 0 = slot free, 1 = in flight
 *   damage    : HP removed on hit
 *   dx, dy    : velocity per move tick
 *   move_timer: per-bullet countdown; when 0 the bullet steps by (dx,dy)
 *   is_player : 1 = player bullet (hits enemies), 0 = enemy bullet (hits player)
 */
typedef struct {
    int x;
    int y;
    int active;
    int damage;
    int dx;
    int dy;
    int move_timer;
    int is_player;
} Bullet;

void bullets_init(void);

/*
 * bullet_spawn — fires a bullet from any source.
 *   is_player = 1  → player bullet (travels up, hits enemies)
 *   is_player = 0  → enemy bullet  (travels down, hits player)
 * Returns 1 if a free slot was found, 0 if pool is full.
 */
int  bullet_spawn(int x, int y, int dx, int dy, int damage, int is_player);
void bullets_update(unsigned int frame);
void bullets_draw(void);

/*
 * bullets_check_hit — checks enemy bullets against the player plane.
 * Player bullets (is_player==1) are skipped.
 * Returns total damage dealt; consumed bullets are deactivated.
 */
int  bullets_check_hit(int px, int py);

/*
 * bullets_consume_player_hits — called by enemy.c for each active enemy.
 * Checks if any player bullet overlaps position (ex, ey).
 * On match: bullet deactivated, damage accumulated and returned.
 */
int  bullets_consume_player_hits(int ex, int ey);

#endif /* BULLET_H */