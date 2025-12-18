// src/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

// Expose player state if other modules need it (bullets spawn, enemies chase, etc.)
extern Sprite wizard;

void player_init(void);
void player_update(void); // reads input, moves wizard, fires bullets
void player_draw(void);   // draws wizard metasprite

#endif
