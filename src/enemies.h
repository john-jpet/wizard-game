// src/enemies.h
#ifndef ENEMIES_H
#define ENEMIES_H

#include "game.h"

// actual enemy array lives in enemies.c
extern Enemy enemies[MAX_ENEMIES];

// lane table lives in enemies.c
extern signed char lane_enemy[LANES];

extern unsigned char active_enemies_count;


void enemies_init(void);
void spawn_enemy(unsigned char x, unsigned char y, unsigned char type);
void enemies_update_and_draw(void);
void clear_enemy_bullets(void);
unsigned char count_active_enemies(void);  // Helper for wave spawning

// collision helper table (called once per frame, after enemies_update)
void build_lane_enemy_table(void);

// used by bullets code
unsigned char enemy_lane_from_center(unsigned char ex);

#endif
