// src/game.c
/* Game state management and global functions */
#include "game.h"
#include "enemies.h"
#include "player.h"
#include "gfx.h"
#include "bullets.h"
#include "pickup.h"

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
  unsigned char i;
  unsigned char d;

  sfx_play(SFX_SCREEN_CLEAR, 0);
  
  if (nuke_timer) return;        // already nuking
  nuke_timer = NUKE_FLASH_DURATION;

  // Kill each enemy properly (with score and pickups)
  for (i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      enemies[i].active = 0;
      active_enemies_count--;
      score_add(ENEMY_KILL_SCORE);
      
      // Roll for pickup drops
      d = roll_drop_pickup();
      if (d == 1) spawn_pickup(enemies[i].x + 4, enemies[i].y, 0);      // heart
      else if (d == 2) spawn_pickup(enemies[i].x + 4, enemies[i].y, 1); // star
    }
  }
  
  clear_enemy_bullets();
  player_use_mp(NUKE_MP_COST);
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