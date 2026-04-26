#ifndef ENEMY_H
#define ENEMY_H

/*
 * EnemyType — distinguishes AI behaviour and shoot patterns.
 *   ENEMY_NOOB : moves straight down, fires one bullet vertically.
 *   ENEMY_MID  : moves straight down, fires a 5-bullet spread fan.
 *
 * Adding a new type = one new enum value + one new case in enemies_update().
 */
typedef enum { ENEMY_NOOB = 0, ENEMY_MID } EnemyType;

/*
 * Enemy — a single enemy plane on screen.
 *   x, y         : current position (nose / centre cell)
 *   active       : 0 = slot free, 1 = enemy alive on screen
 *   health       : HP remaining (unused for collisions until V5 player shoots)
 *   shoot_timer  : counts down to next shot; reset to type constant on fire
 *   type         : determines glyph, health, and shoot pattern
 */
typedef struct {
    int       x;
    int       y;
    int       active;
    int       health;
    int       shoot_timer;
    EnemyType type;
} Enemy;

void enemies_init(void);
void enemies_update(unsigned int frame);
void enemies_draw(void);

#endif /* ENEMY_H */
