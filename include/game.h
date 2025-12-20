// include/game.h
#ifndef GAME_H
#define GAME_H

#include "neslib.h"
#include "nesdoug.h"

// constants
#define MAX_BULLETS 4
#define MAX_ENEMIES 6

#define BULLET_TILE 0x15
#define BULLET_PAL  1
#define ENEMY_PAL   2

#define ENEMY_SPAWN_MIN_X  8
#define ENEMY_SPAWN_MAX_X  232

#define LANES 16
#define LANE_SHIFT 4  // 16px lanes


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
} Enemy;

typedef struct {
  unsigned char x, y, width, height;
  signed char vx, vy;
  unsigned char active;
} Fireball;

typedef struct {
  unsigned char x, y, width, height;
  signed char vx, vy;
  unsigned char active;
} EnemyFireball;

extern unsigned int score;

void score_add(unsigned int amount);
void score_reset(void);


#endif
