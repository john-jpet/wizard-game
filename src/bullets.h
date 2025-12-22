// src/bullets.h
#ifndef BULLETS_H
#define BULLETS_H

#include "game.h"

#define DROP_HEART_CHANCE 8   // 1 in 8
#define DROP_STAR_CHANCE  16  // 1 in 16

extern Fireball bullets[MAX_BULLETS];
extern EnemyFireball ebullets[MAX_BULLETS];

void bullets_init(void);
void ebullets_init(void);
void fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy);
void enemy_fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy);
void player_bullets_update_collide_draw(void);
void enemy_bullets_update_collide_draw(void);
unsigned char roll_drop_pickup(void);

#endif