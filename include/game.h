// include/game.h
#ifndef GAME_H
#define GAME_H

#include "neslib.h"
#include "nesdoug.h"

// constants
#define MAX_BULLETS 4
#define MAX_ENEMY_BULLETS 8
#define MAX_ENEMIES 4

#define BULLET_TILE 0x15
#define BULLET_PAL  1
#define ENEMY_PAL   2
#define WARLOCK_PAL 3

#define ENEMY_SPAWN_MIN_X  8
#define ENEMY_SPAWN_MAX_X  232

#define LANES 16
#define LANE_SHIFT 4  // 16px lanes

// Game balance constants
#define ENEMY_KILL_SCORE 100
#define STAR_PICKUP_SCORE 500
#define NUKE_MP_COST 3
#define NUKE_FLASH_DURATION 12


// structs
typedef struct {
  unsigned char x, y, width, height;
} Sprite;

typedef struct {
  unsigned char x, y, width, height;
  signed char vx, vy;
  unsigned char hp;
  unsigned char active;
  unsigned char anim;
  unsigned char type;
  unsigned char move_counter;
  unsigned char _padding[5];  // Pad to 16 bytes for fast array access
} Enemy;

typedef struct {
  unsigned char x, y, width, height;
  signed char vx, vy;
  unsigned char active;
  unsigned char _padding;  // Pad to 8 bytes for fast array access
} Fireball;

typedef struct {
  unsigned char x, y, width, height;
  signed char vx, vy;
  unsigned char active;
  unsigned char palette;  // Palette for drawing (was _padding)
} EnemyFireball;

extern unsigned int score;

void score_add(unsigned int amount);
void score_reset(void);

void nuke_trigger(void);
void nuke_update_fx(void);

#endif
