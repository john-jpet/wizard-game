
#include "hud.h"

#define HEART_TILE 0x80
#define EMPTY_HEART_TILE 0x81
#define STAR_TILE 0x82
#define EMPTY_STAR_TILE 0x83

unsigned char i = 0;
void hud_set_hp(unsigned char hp) {
  // "HP:" at (1,1) and digit at (4,1)
  one_vram_buffer('H', NTADR_A(1, 1));
  one_vram_buffer('P', NTADR_A(2, 1));
  one_vram_buffer(':', NTADR_A(3, 1));
  for(i = 0; i < 3; i++) {
    if(i < hp) {
      one_vram_buffer(HEART_TILE, NTADR_A(4 + i, 1));
    } else {
      one_vram_buffer(EMPTY_HEART_TILE, NTADR_A(4 + i, 1));
    }
  }
}

void hud_set_mp(unsigned char mp) {
  // "MP:" at (1,2) and digit at (4,2)
  one_vram_buffer('M', NTADR_A(1, 2));
  one_vram_buffer('P', NTADR_A(2, 2));
  one_vram_buffer(':', NTADR_A(3, 2));
  for(i = 0; i < 5; i++) {
    if(i < mp) {
      one_vram_buffer(STAR_TILE, NTADR_A(4 + i, 2));
    } else {
      one_vram_buffer(EMPTY_STAR_TILE, NTADR_A(4 + i, 2));
    }
  }
}

void hud_set_score(unsigned int score) {
  //"Score:" at (12,1) and 5-digit score at (18,1)
  multi_vram_buffer_horz("SCORE:", 6, NTADR_A(12, 1));
  one_vram_buffer((unsigned char)('0' + (score / 10000) % 10), NTADR_A(18, 1));
  one_vram_buffer((unsigned char)('0' + (score / 1000) % 10), NTADR_A(19, 1));
  one_vram_buffer((unsigned char)('0' + (score / 100) % 10), NTADR_A(20, 1));
  one_vram_buffer((unsigned char)('0' + (score / 10) % 10), NTADR_A(21, 1));
  one_vram_buffer((unsigned char)('0' + (score / 1) % 10), NTADR_A(22, 1));

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