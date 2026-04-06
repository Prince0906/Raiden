# RAIDEN-C

> A terminal-based vertical shooter built entirely in C — no standard math, string, or memory libraries. Just raw systems programming and a flying plane.

Inspired by the 1990 arcade classic Raiden. Built from scratch as a learning project in pure C, with five custom foundational libraries powering every pixel, calculation, and keypress.

---

## What this game is

An infinite vertical shooter. Your plane flies. Bullets fall. Enemies attack. You dodge, shoot, and survive for as long as you can — like Subway Surfers, but with air combat and explosions.

There is no final level. There is no winning. There is only a higher score.

---

## Project rules (self-imposed constraints)

These constraints exist to force deep understanding of how computers actually work.

| Rule | What it means |
|------|---------------|
| No `<string.h>` | No `strlen`, `strcpy`, `strcmp` — implement them yourself |
| No `<math.h>` | No `sqrt`, `abs`, `pow` — implement integer arithmetic from scratch |
| No `malloc` / `free` | Claim one big memory pool at startup, manage it yourself |
| `<stdio.h>` allowed | For terminal I/O only — `printf`, `getchar` |
| `<stdlib.h>` allowed | For `exit()` and the single initial pool allocation |
| No hard-coded values | Every dimension, speed, and limit is a named constant |

---

## Foundational libraries

These five libraries are built before any game code is written. Each has its own `.c` and `.h` file.

```
libs/
├── math.c / math.h       — multiply, divide, modulo, abs, clamp, in_bounds
├── string.c / string.h   — length, copy, compare, split, int_to_str
├── memory.c / memory.h   — virtual RAM pool, my_alloc(), my_dealloc()
├── screen.c / screen.h   — ANSI terminal renderer, cursor control, draw calls
└── keyboard.c / keyboard.h — non-blocking input, raw terminal mode
```

The libraries form a pipeline. Input flows through all five layers every frame:

```
keyboard.c  →  string.c  →  memory.c  →  math.c  →  screen.c
  (capture)     (parse)     (allocate)   (compute)   (render)
```

---

## Versioned roadmap

Each version is a self-contained, playable milestone. Nothing in V2 is built until V1 is stable. Nothing in V3 until V2 is stable.

---

### V1 — The plane exists

**Goal:** A plane appears on screen and moves. That's it. The full library pipeline is validated end-to-end.

**What's in it:**
- Terminal renders at 80×24, fixed size
- A scrolling star field (vertical, wrapping) — the sense of flight
- Player plane drawn as an ASCII character at a starting position
- WASD / arrow key movement in four directions
- Hard boundaries: `clamp()` from `math.c` prevents leaving the screen
- HUD bar: score (`0`) and lives (`3`) drawn via `int_to_str()` + `screen.c`
- Clean exit restores terminal state (cursor visible, cooked mode restored)

**What this validates:**
- `keyboard.c` raw mode and non-blocking input working
- `screen.c` double-buffer anti-flicker rendering working
- `math.c` boundary clamping working
- `string.c` integer-to-string for HUD working
- `memory.c` pool allocation working (plane struct allocated from pool)

**Definition of done:** Plane moves smoothly in all four directions. No flicker. Terminal restores on Ctrl+C.

---

### V2 — Bullets fall, death is possible

**Goal:** The game can be lost. Bullets rain from the top of the screen at random positions and speeds. If one touches the plane, the game ends.

**What's added:**
- Bullet entities allocated via `my_alloc()` on spawn, freed via `my_dealloc()` on exit or hit
- Bullets spawn at random X positions along the top row
- Each bullet has its own speed (slow / medium / fast — values from named constants)
- Collision detection: `in_bounds()` from `math.c` checks if a bullet overlaps the plane
- On collision → GAME\_OVER state, render game-over screen, return to title on keypress
- Game state machine: `TITLE → PLAYING → GAME_OVER → TITLE`
- Score increments over time (survival score)

**Key architectural moment:** Every bullet spawn is a `my_alloc()`. Every bullet that leaves the screen or hits the player is a `my_dealloc()`. This is the zero-leak requirement in action.

**Definition of done:** Bullets fall, plane can die, game resets cleanly with zero memory leaks.

---

### V3 — Health, not instant death

**Goal:** The plane has health points. Bullets deal varying damage. The game becomes about survival under sustained pressure, not just avoiding a single touch.

**What's added:**
- `int health` field on the Player struct (e.g. max 100)
- Bullets have an `int damage` field — slow bullets deal less, fast bullets deal more
- On collision: `health -= bullet.damage` instead of instant death
- Player "flashes" on hit (invincibility frames via a countdown counter)
- Health bar rendered on HUD alongside score and lives
- Death only triggers when `health <= 0`
- Health values and damage ranges are named constants — never literals

**What this validates:** `math.c` is now doing damage arithmetic and health clamping. The HUD renders a third dynamic value. The invincibility frame system eliminates multi-hit bugs.

**Definition of done:** Plane survives multiple hits. Damage varies visibly by bullet speed. Health bar updates correctly every frame.

---

### V4 — Enemies arrive

**Goal:** Replace random bullets with intelligent enemy planes. Each enemy has defined movement and shooting behaviour. The game has tactical depth.

**What's added:**
- Enemy struct with fields: `x`, `y`, `type` (enum), `health`, `shoot_timer`, `move_pattern`
- Enemy type enum: `ENEMY_NOOB`, `ENEMY_MID` (extensible — adding a new type is one enum value + one switch case)
- All enemies enter from the top, move downward
- **Noob enemy:** moves straight down, fires single bullets vertically downward at intervals
- **Mid enemy:** moves straight down, fires 5–6 bullets simultaneously in a spread pattern (fan of directions) — bullets computed using precomputed direction offsets in `math.c`
- Enemies shoot continuously as long as they are on screen
- Enemy bullets use the same Bullet struct as V2 — `dy = +1` instead of `-1`
- Enemy exits screen at bottom → `my_dealloc()`, new enemy spawns
- Fixed wave spawner: every N frames, an enemy spawns at a random X on row 0

**Key architectural decision:** Bullet direction (`dy`) and damage are fields on the Bullet struct — not assumptions. The same bullet system handles all sources.

**Definition of done:** Two distinct enemy types appear with different shooting patterns. Enemies scroll off screen cleanly. Memory usage stays flat over time.

---

### V5 — The plane fights back

**Goal:** The player can shoot. Enemies can be destroyed. This is the complete game.

**What's added:**
- Player fires bullets upward on spacebar press
- Player bullets: `dy = -1`, damage = `PLAYER_BULLET_DAMAGE` (named constant)
- Bullet cap: `MAX_PLAYER_BULLETS` active at once — pool of bullet slots, `active` flag per slot
- Enemy health decrements when hit by player bullet
- Enemy death: `my_dealloc()` enemy, increment score by `ENEMY_SCORE_VALUE`
- Score increases significantly faster from kills than from survival time
- Difficulty scales infinitely: every N seconds, `difficulty++` increases enemy speed, spawn rate, and bullet frequency — all computed via `math.c` using the difficulty integer

**What this completes:** The full game loop. Plane dodges, plane shoots, enemies die, score climbs, difficulty rises. The game runs forever.

**Definition of done:** Player can kill enemies. Score reflects kills. Game runs for 10+ minutes without crash, flicker, or memory growth.

---

### V6 — Collectibles (stretch goal)

**Goal:** Reward skilled play with health restoration and weapon upgrades. The game has build-your-run depth.

**What's added:**
- Collectible entity: `type` (HEALTH\_BLOB or UPGRADE\_GEM), `x`, `y`, spawned randomly on map when an enemy dies (probability-based, using `my_mod()`)
- **Health blob:** on player collection, `health = PLAYER_MAX_HEALTH` (full restore)
- **Upgrade gem:** on collection, `weapon_level++` on player struct — increases bullet damage or adds spread shots
- Weapon level is capped at `MAX_WEAPON_LEVEL` (named constant)
- Collectibles drift slowly downward; if not collected, they exit and are freed
- Rendered with distinct glyphs to distinguish them from bullets

**Key architectural moment:** Collectible spawn uses `my_alloc()`. Collection and screen-exit both use `my_dealloc()`. Weapon level changes behaviour inside the existing bullet-fire logic via a switch on `weapon_level`.

**Definition of done:** Collectibles spawn, move, are collectible, and are freed correctly. Weapon upgrades are visibly more powerful. Zero new memory leaks introduced.

---

## Game constants (defined before writing any code)

```c
/* screen */
#define SCREEN_W        80
#define SCREEN_H        24
#define PLAY_X_MIN      1
#define PLAY_X_MAX      (SCREEN_W - 2)
#define PLAY_Y_MIN      1
#define PLAY_Y_MAX      (SCREEN_H - 3)   /* bottom 2 rows reserved for HUD */

/* player */
#define PLAYER_MAX_HEALTH     100
#define PLAYER_START_LIVES    3
#define PLAYER_BULLET_DAMAGE  20
#define PLAYER_INVINCIBLE_FRAMES 40
#define MAX_PLAYER_BULLETS    5
#define MAX_WEAPON_LEVEL      3

/* enemies */
#define MAX_ENEMIES           8
#define ENEMY_NOOB_HEALTH     20
#define ENEMY_MID_HEALTH      50
#define ENEMY_SPAWN_INTERVAL  60       /* frames between spawns */
#define ENEMY_SCORE_VALUE     100

/* bullets */
#define MAX_ENEMY_BULLETS     24
#define BULLET_DMG_SLOW       5
#define BULLET_DMG_FAST       20

/* timing */
#define TARGET_FPS            20
#define FRAME_MS              (1000 / TARGET_FPS)
#define DIFFICULTY_INTERVAL   300      /* frames between difficulty increments */
```

---

## File structure

```
raiden-c/
├── README.md
├── Makefile
├── libs/
│   ├── math.c / math.h
│   ├── string.c / string.h
│   ├── memory.c / memory.h
│   ├── screen.c / screen.h
│   └── keyboard.c / keyboard.h
└── src/
    ├── main.c          — entry point, game loop, timing, state machine
    ├── config.h        — all named constants (included everywhere)
    ├── player.c/.h     — player struct, movement, fire, health
    ├── enemy.c/.h      — enemy struct, AI movement, shoot behaviour
    ├── bullet.c/.h     — bullet struct, move, alloc/dealloc lifecycle
    ├── collision.c/.h  — AABB checks using math.c in_bounds()
    ├── renderer.c/.h   — orchestrates all draw calls each frame
    └── level.c/.h      — wave spawning, difficulty scaling, score
```

---

## Build and run

```bash
# compile
make

# run
./raiden

# run with a specific version tag (once tagged in git)
git checkout v1 && make && ./raiden
```

---

## Version history

| Tag | Description | Status |
|-----|-------------|--------|
| `v1` | Plane + movement + star field | Planned |
| `v2` | Random bullets + death + game over | Planned |
| `v3` | Health system + damage variants + invincibility | Planned |
| `v4` | Enemy planes + AI patterns + enemy shooting | Planned |
| `v5` | Player shooting + enemy death + infinite scaling | Planned |
| `v6` | Collectibles + weapon upgrades | Stretch goal |

---

## Why build this way?

Standard library functions exist for a reason — they are fast, tested, and portable. Avoiding them here is not because they are bad. It is because understanding what they do under the hood makes you a fundamentally better programmer.

When you implement `my_alloc()`, you understand what memory fragmentation actually is. When you implement `int_to_str()`, you understand why C strings work the way they do. When you implement the game loop timing yourself, you understand why games have frame budgets.

Every constraint in this project is a teacher.

---

*Started: 2026 — Building in public.*
