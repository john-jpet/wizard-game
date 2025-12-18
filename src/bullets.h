// src/bullets.h
#ifndef BULLETS_H
#define BULLETS_H

#include "game.h"

extern Fireball bullets[MAX_BULLETS];
extern EnemyFireball ebullets[MAX_BULLETS];

void bullets_init(void);
void ebullets_init(void);
void fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy);
void enemy_fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy);
void player_bullets_update_collide_draw(void);
void enemy_bullets_update_collide_draw(void);

#endif