#ifndef HUD_H
#define HUD_H

#include "player.h"

void hud_init(void);          // draws static labels once, clears cache
void hud_set_hp(unsigned char hp);
void hud_set_score(unsigned int score);
void hud_set_mp(unsigned char mp);
void hud_draw_if_dirty(void); // does vram_buffer writes

#endif