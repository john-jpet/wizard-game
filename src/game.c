
#include "game.h"
#include "enemies.h"
#include "player.h"
#include "gfx.h"

unsigned int score = 0;

void score_reset() {
  score = 0;
}

void score_add(unsigned int amount) {
  score += amount;
}

static unsigned char nuke_timer = 0;

// White flash palette (all white)
static const unsigned char flash_palette_bg[] = {
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30
};

static const unsigned char flash_palette_sp[] = {
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30
};

void nuke_trigger(void) {
  if (nuke_timer) return;        // already nuking
  nuke_timer = 12;               // effect lasts 12 frames

  kill_all_enemies();
  clear_enemy_bullets();
  player_use_mp(3);
}

void nuke_update_fx(void) {
  if (!nuke_timer) return;

  // Flash every other frame by swapping palettes
  if (nuke_timer & 1) {
    pal_bg(flash_palette_bg);
    pal_spr(flash_palette_sp);
  } else {
    pal_bg(palette_bg);
    pal_spr(palette_sp);
  }

  nuke_timer--;

  if (!nuke_timer) {
    // Restore normal palettes when done
    pal_bg(palette_bg);
    pal_spr(palette_sp);
  }
}
