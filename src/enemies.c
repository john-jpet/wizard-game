// src/enemies.c
/* Enemy management and collision detection */
#include "enemies.h"
#include "bullets.h"
#include "player.h"
#include "gfx.h"

#define ENEMY_BOTTOM_Y 0xD8
#define ANIM_CYCLE 60
#define ANIM_SWITCH 30
#define WARLOCK_ANIM_CYCLE 120  // Warlock has longer animation cycle
#define WARLOCK_FIRE_FRAME 100   // Warlock fires at this frame
#define WARLOCK_FIRE_DURATION 30 // Warlock stays in fire pose for 30 frames

// Screen boundaries for fire spirit weaving
#define FIRE_SPIRIT_MIN_X 8
#define FIRE_SPIRIT_MAX_X 232

Enemy enemies[MAX_ENEMIES];
signed char lane_enemy[LANES];

// lane helper
unsigned char enemy_lane_from_center(unsigned char ex) {
  return (unsigned char)((ex + 8) >> LANE_SHIFT);
}

void enemies_init(void) {
  unsigned char i;
  for (i = 0; i < MAX_ENEMIES; i++) enemies[i].active = 0;
}

void clear_enemy_bullets(void) {
  unsigned char i;
  for (i = 0; i < MAX_BULLETS; i++) ebullets[i].active = 0;
}

void spawn_enemy(unsigned char x, unsigned char y, unsigned char type) {
  unsigned char i;
  for (i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].active = 1;
      enemies[i].x = x;
      enemies[i].y = y;
      // Golem is 24x32 (3 tiles wide, 4 tiles tall), small slimes are 8x8, others are 16x16
      if (type == 5) {
        enemies[i].width = 8;
        enemies[i].height = 8;
      } else if (type == 6) {
        enemies[i].width = 24;  // Golem is 3 tiles wide
        enemies[i].height = 32;  // Golem is 4 tiles tall
      } else {
        enemies[i].width = 16;
        enemies[i].height = 16;
      }
      if(type == 3) enemies[i].vx = 1;  // fire spirit weaves
      else enemies[i].vx = 0;
      enemies[i].vy = 0;
      // Type 0 (slow imp): 3 HP
      // Type 1 (fast imp): 1 HP
      // Type 2 (warlock): 5 HP
      // Type 3 (fire spirit): 2 HP
      // Type 4 (large slime): 3 HP, splits into 2 small slimes
      // Type 5 (small slime): 1 HP
      // Type 6 (tank imp): 7 HP, slow, fires large bullets
      if (type == 0) enemies[i].hp = 3;
      else if (type == 2) enemies[i].hp = 5;
      else if (type == 3) enemies[i].hp = 2;
      else if (type == 4) enemies[i].hp = 3;
      else if (type == 6) enemies[i].hp = 7;
      else enemies[i].hp = 1;
      enemies[i].anim = rand8() % ANIM_CYCLE;
      enemies[i].type = type;
      enemies[i].move_counter = 0;
      return;
    }
  }
}

void enemies_update_and_draw(void) {
  unsigned char i;
  unsigned char is_firing;
  
  for (i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;

    // Fire spirit weaving logic (must happen before vertical movement)
    if(enemies[i].type == 3) {
      enemies[i].x += enemies[i].vx;
      
      // Bounce off screen edges with proper clamping
      if (enemies[i].x <= FIRE_SPIRIT_MIN_X) {
        enemies[i].x = FIRE_SPIRIT_MIN_X;
        enemies[i].vx = 1;  // Move right
      } else if (enemies[i].x >= FIRE_SPIRIT_MAX_X) {
        enemies[i].x = FIRE_SPIRIT_MAX_X;
        enemies[i].vx = -1;  // Move left
      }
    }

    // UPDATE: Frame-based movement for fractional speeds
    enemies[i].move_counter++;
    
    // Check if warlock is currently in firing animation (long duration)
    is_firing = (enemies[i].type == 2 && enemies[i].anim >= WARLOCK_FIRE_FRAME && enemies[i].anim < (WARLOCK_FIRE_FRAME + WARLOCK_FIRE_DURATION));
    
    // Type 0 (slow): move 1 pixel every 2 frames = 0.5 px/frame
    // Type 1 (fast): move 1 pixel every frame = 1 px/frame
    // Type 2 (warlock): move 1 pixel every 4 frames = 0.25 px/frame, STOP when firing
    // Type 3 (fire spirit): move 1 pixel every 2 frames = 0.5 px/frame (while weaving at 1 px/frame)
    // Type 4 (large slime): move 1 pixel every 2 frames = 0.5 px/frame
    // Type 5 (small slime): move 1 pixel every frame = 1 px/frame
    // Type 6 (tank imp): move 1 pixel every 3 frames = 0.33 px/frame (very slow)
    if (enemies[i].type == 0) {
      if (enemies[i].move_counter >= 2) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else if (enemies[i].type == 2) {
      // Warlock: stops moving when firing
      if (!is_firing && enemies[i].move_counter >= 4) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else if (enemies[i].type == 3) {
      // Fire spirit: moves down every 2 frames (same vertical speed as type 0, but weaves)
      if (enemies[i].move_counter >= 2) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else if (enemies[i].type == 4) {
      // Large slime: slow movement
      if (enemies[i].move_counter >= 2) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else if (enemies[i].type == 6) {
      // Tank imp: very slow, heavy movement
      if (enemies[i].move_counter >= 3) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else {  // type 1 and type 5 (small slime)
      enemies[i].y++;
      enemies[i].move_counter = 0;
    }
    
    if (enemies[i].y >= ENEMY_BOTTOM_Y) {
      enemies[i].active = 0;
      player_take_damage();
      continue;
    }

    enemies[i].anim++;
    
    // Different animation cycles for different enemy types
    if (enemies[i].type == 2) {
      if (enemies[i].anim >= WARLOCK_ANIM_CYCLE) enemies[i].anim = 0;
    } else {
      if (enemies[i].anim >= ANIM_CYCLE) enemies[i].anim = 0;
    }

    // Fire bullets at specific animation frames
    if (enemies[i].type == 0 && enemies[i].anim == ANIM_SWITCH) {
      enemy_fire_bullet(enemies[i].x + 4, enemies[i].y + 8, 0, 3, ENEMY_PAL);
    } else if (enemies[i].type == 2 && enemies[i].anim == WARLOCK_FIRE_FRAME) {
      // Warlock fires 3 bullets in a spread at frame 100 (out of 120) with purple palette
      enemy_fire_bullet(enemies[i].x + 4, enemies[i].y, -1, 2, WARLOCK_PAL);
      enemy_fire_bullet(enemies[i].x + 4, enemies[i].y, 0, 2, WARLOCK_PAL);
      enemy_fire_bullet(enemies[i].x + 4, enemies[i].y, 1, 2, WARLOCK_PAL);
    } 
    
    // DRAW: Combined in same loop to save iteration overhead
    if (enemies[i].type == 2) {
      // Warlock animation: idle -> walk -> fire
      if (is_firing) {
        oam_meta_spr(enemies[i].x, enemies[i].y, warlock_fire);
      } else if (enemies[i].anim < 60) {
        oam_meta_spr(enemies[i].x, enemies[i].y, warlock);
      } else {
        oam_meta_spr(enemies[i].x, enemies[i].y, warlock_1);
      }
    } else if (enemies[i].type == 3) {
      // Fire spirit - alternates between two flame frames
      if (enemies[i].anim < ANIM_SWITCH) {
        oam_meta_spr(enemies[i].x, enemies[i].y, fire_spirit);
      } else {
        oam_meta_spr(enemies[i].x, enemies[i].y, fire_spirit_1);
      }
    } else if (enemies[i].type == 4) {
      // Large slime - alternates between two frames
      if (enemies[i].anim < ANIM_SWITCH) {
        oam_meta_spr(enemies[i].x, enemies[i].y, slime_large);
      } else {
        oam_meta_spr(enemies[i].x, enemies[i].y, slime_large_1);
      }
    } else if (enemies[i].type == 5) {
      // Small slime - single 8x8 sprite
      oam_spr(enemies[i].x, enemies[i].y, 0x0A, ENEMY_PAL);
    } else if (enemies[i].type == 6) {
      // Golem - alternates between two frames
      if (enemies[i].anim < ANIM_SWITCH) {
        oam_meta_spr(enemies[i].x, enemies[i].y, golem);
      } else {
        oam_meta_spr(enemies[i].x, enemies[i].y, golem_1);
      }
    } else {
      // Types 0 and 1 (imps) - animate normally
      if (enemies[i].anim < ANIM_SWITCH) {
        if (enemies[i].type == 0) oam_meta_spr(enemies[i].x, enemies[i].y, imp);
        else                      oam_meta_spr(enemies[i].x, enemies[i].y, diveimp);
      } else {
        if (enemies[i].type == 0) oam_meta_spr(enemies[i].x, enemies[i].y, imp1);
        else                      oam_meta_spr(enemies[i].x, enemies[i].y, diveimp1);
      }
    }
  }
}

void build_lane_enemy_table(void) {
  unsigned char l, i;

  for (l = 0; l < LANES; l++) lane_enemy[l] = -1;

  for (i = 0; i < MAX_ENEMIES; i++) {
    unsigned char lane;
    if (!enemies[i].active) continue;

    lane = enemy_lane_from_center(enemies[i].x);

    if (lane_enemy[lane] < 0 || enemies[i].y > enemies[(unsigned char)lane_enemy[lane]].y) {
      lane_enemy[lane] = (signed char)i;
    }
  }
}
