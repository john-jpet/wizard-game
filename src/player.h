// src/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

// Expose player state if other modules need it (bullets spawn, enemies chase, etc.)
extern Sprite wizard;

void player_init(void);
void player_update(void); // reads input, moves wizard, fires bullets
void player_draw(void);   // draws wizard metasprite

// health
unsigned char player_get_hp(void);
unsigned char player_is_dead(void);
unsigned char player_is_invincible(void);
void player_take_damage(void);

#endif
