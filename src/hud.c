//src/hud.c
/* HUD management: health, mana, score display */
#include "hud.h"

#define HEART_TILE 0x80
#define EMPTY_HEART_TILE 0x81
#define STAR_TILE 0x82
#define EMPTY_STAR_TILE 0x83

unsigned char i = 0;
void hud_set_hp(unsigned char hp) {
  // Only update hearts, NOT labels
  for(i = 0; i < 3; i++) {
    if(i < hp) {
      one_vram_buffer(HEART_TILE, NTADR_A(4 + i, 1));
    } else {
      one_vram_buffer(EMPTY_HEART_TILE, NTADR_A(4 + i, 1));
    }
  }
}

void hud_set_mp(unsigned char mp) {
  // Only update stars, NOT labels
  for(i = 0; i < 5; i++) {
    if(i < mp) {
      one_vram_buffer(STAR_TILE, NTADR_A(4 + i, 2));
    } else {
      one_vram_buffer(EMPTY_STAR_TILE, NTADR_A(4 + i, 2));
    }
  }
}

void hud_set_score(unsigned int score) {
  // Only update score digits, NOT "SCORE:" label
  // Optimize: use repeated subtraction instead of division for better performance
  unsigned int temp = score;
  unsigned char d;
  
  // 10000s place
  d = 0;
  while (temp >= 10000) { temp -= 10000; d++; }
  one_vram_buffer((unsigned char)('0' + d), NTADR_A(18, 1));
  
  // 1000s place  
  d = 0;
  while (temp >= 1000) { temp -= 1000; d++; }
  one_vram_buffer((unsigned char)('0' + d), NTADR_A(19, 1));
  
  // 100s place
  d = 0;
  while (temp >= 100) { temp -= 100; d++; }
  one_vram_buffer((unsigned char)('0' + d), NTADR_A(20, 1));
  
  // 10s place
  d = 0;
  while (temp >= 10) { temp -= 10; d++; }
  one_vram_buffer((unsigned char)('0' + d), NTADR_A(21, 1));
  
  // 1s place
  one_vram_buffer((unsigned char)('0' + temp), NTADR_A(22, 1));
}

void hud_init(void) {
  // draw static labels once
  one_vram_buffer('H', NTADR_A(1, 1));
  one_vram_buffer('P', NTADR_A(2, 1));
  one_vram_buffer(':', NTADR_A(3, 1));
  one_vram_buffer('M', NTADR_A(1, 2));
  one_vram_buffer('P', NTADR_A(2, 2));
  one_vram_buffer(':', NTADR_A(3, 2));
  multi_vram_buffer_horz("SCORE:00000", 11, NTADR_A(12, 1));
}