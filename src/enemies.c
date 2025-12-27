// src/enemies.c
/* Enemy management and collision detection */
#include "enemies.h"
#include "bullets.h"
#include "player.h"
#include "gfx.h"

#define ENEMY_BOTTOM_Y 0xD8
#define ANIM_CYCLE 60
#define ANIM_SWITCH 30

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
      enemies[i].width = 16;
      enemies[i].height = 16;
      enemies[i].vx = 0;
      enemies[i].vy = 0;
      enemies[i].hp = (type == 0) ? 3 : 1;
      enemies[i].anim = rand8() % ANIM_CYCLE;
      enemies[i].type = type;
      enemies[i].move_counter = 0;
      return;
    }
  }
}

void enemies_update_and_draw(void) {
  unsigned char i;
  for (i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;

    // UPDATE: Frame-based movement for fractional speeds
    enemies[i].move_counter++;
    
    // Type 0 (slow): move 1 pixel every 2 frames = 0.5 pixels/frame
    // Type 1 (fast): move 1 pixel every frame = 1 pixel/frame
    if (enemies[i].type == 0) {
      if (enemies[i].move_counter >= 2) {
        enemies[i].y++;
        enemies[i].move_counter = 0;
      }
    } else {  // type 1
      enemies[i].y++;
      enemies[i].move_counter = 0;
    }
    
    if (enemies[i].y >= ENEMY_BOTTOM_Y) {
      enemies[i].active = 0;
      player_take_damage();
      continue;
    }

    enemies[i].anim++;
    if (enemies[i].anim >= ANIM_CYCLE) enemies[i].anim = 0;

    if (enemies[i].anim == ANIM_SWITCH && enemies[i].type == 0) {
      enemy_fire_bullet(enemies[i].x + 4, enemies[i].y + 8, 0, 3);
    }
    
    // DRAW: Combined in same loop to save iteration overhead
    if (enemies[i].anim < ANIM_SWITCH) {
      if (enemies[i].type == 0) oam_meta_spr(enemies[i].x, enemies[i].y, imp);
      else                      oam_meta_spr(enemies[i].x, enemies[i].y, diveimp);
    } else {
      if (enemies[i].type == 0) oam_meta_spr(enemies[i].x, enemies[i].y, imp1);
      else                      oam_meta_spr(enemies[i].x, enemies[i].y, diveimp1);
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
