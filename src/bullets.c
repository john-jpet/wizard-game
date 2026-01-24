// src/bullets.c
/* Bullet management and collision detection */
#include "game.h"
#include "bullets.h"
#include "enemies.h"
#include "player.h"
#include "pickup.h"
#include "gfx.h"

Fireball bullets[MAX_BULLETS];
EnemyFireball ebullets[MAX_ENEMY_BULLETS];
Fireball superbullet;

void bullets_init(void) {
    unsigned char i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    superbullet.active = 0;
}

void ebullets_init(void) {
    unsigned char i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        ebullets[i].active = 0;
    }
    
}

void fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy) {
    unsigned char i;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].width = 8;
            bullets[i].height = 8;
            bullets[i].vx = vx;
            bullets[i].vy = vy;
            sfx_play(SFX_JUMP, 0);
            return;
        }
    }
}

void fire_super(unsigned char x, unsigned char y, signed char vx, signed char vy) {
  if (superbullet.active) return;
  player_use_mp(1);
  superbullet.active = 1;
  superbullet.x = x;
  superbullet.y = y;
  superbullet.width = 16;
  superbullet.height = 16;
  superbullet.vx = vx;
  superbullet.vy = vy;
}


void enemy_fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy, unsigned char palette) {
    unsigned char i;
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!ebullets[i].active) {
            ebullets[i].active = 1;
            ebullets[i].x = x;
            ebullets[i].y = y;
            // Golem heavy bullets (palette = 0xFF) are 16x16, others are 4x4
            if (palette == 0xFF) {
                ebullets[i].width = 16;
                ebullets[i].height = 16;
            } else {
                ebullets[i].width = 4;
                ebullets[i].height = 4;
            }
            ebullets[i].vx = vx;
            ebullets[i].vy = vy;
            ebullets[i].palette = palette;
            return;
        }
    }
}

static unsigned char bullet_lane_from_center(unsigned char bx) {
    return (unsigned char)((bx + 4) >> LANE_SHIFT);
}

unsigned char roll_drop_pickup(void) {
    unsigned char r = rand8();

  // star rarer than heart
  if ((r & (DROP_STAR_CHANCE - 1)) == 0) return 2;  // 1/16 if DROP_STAR_CHANCE=16
  if ((r & (DROP_HEART_CHANCE - 1)) == 0) return 1; // 1/8  if DROP_HEART_CHANCE=8
  return 0;

}

unsigned char d;
unsigned char pending_kills; // Track number of kills this frame for score
unsigned char pending_pickups[4][3]; // Store up to 4 pending pickups [x, y, type]
unsigned char pending_pickup_count;

void player_bullets_update_collide_draw(void) {
    unsigned char bi;
    signed char ei;
    unsigned char lane;
    unsigned char l2;

    pending_kills = 0; // Reset kill counter
    pending_pickup_count = 0; // Reset pickup queue

    for (bi = 0; bi < MAX_BULLETS; bi++) {

        if (!bullets[bi].active) continue;

        bullets[bi].y += bullets[bi].vy;
        bullets[bi].x += bullets[bi].vx;

        // Early exit if off-screen
        if (bullets[bi].y < 0x10) {
            bullets[bi].active = 0;
            continue;
        }

        lane = bullet_lane_from_center(bullets[bi].x);

        // Check ONLY closest lanes (optimize bounds)
        // Pre-calculate loop bounds to avoid expensive ternary and double-comparison
        {
            unsigned char lane_start = (lane > 0) ? (lane - 1) : 0;
            unsigned char lane_end = (lane < LANES - 1) ? (lane + 1) : (LANES - 1);
            
            for (l2 = lane_start; l2 <= lane_end; l2++) {

            ei = lane_enemy[l2];
            if (ei < 0) continue;
            
            // Quick active check before expensive collision
            if (!enemies[(unsigned char)ei].active) continue;

            if (check_collision((Sprite*)&bullets[bi], (Sprite*)&enemies[(unsigned char)ei])) {
                bullets[bi].active = 0;

                if (enemies[(unsigned char)ei].hp) enemies[(unsigned char)ei].hp--;
                
                // BERSERKER: Golem speeds up aggressively when hit (scales to 0.5 px/frame)
                // Speed progression: 10 -> 6 -> 4 -> 3 -> 2 frames/pixel (0.1 -> 0.5 px/frame)
                if (enemies[(unsigned char)ei].type == 6 && enemies[(unsigned char)ei].hp > 0) {
                  if (enemies[(unsigned char)ei].hp == 6) {
                    enemies[(unsigned char)ei].move_threshold = 6;  // 0.17 px/frame
                  } else if (enemies[(unsigned char)ei].hp == 5) {
                    enemies[(unsigned char)ei].move_threshold = 4;  // 0.25 px/frame
                  } else if (enemies[(unsigned char)ei].hp == 4) {
                    enemies[(unsigned char)ei].move_threshold = 3;  // 0.33 px/frame
                  } else if (enemies[(unsigned char)ei].hp <= 3) {
                    enemies[(unsigned char)ei].move_threshold = 2;  // 0.50 px/frame (capped)
                  }
                }
                
                if (enemies[(unsigned char)ei].hp == 0) {
                    unsigned char enemy_type = enemies[(unsigned char)ei].type;
                    unsigned char enemy_x = enemies[(unsigned char)ei].x;
                    unsigned char enemy_y = enemies[(unsigned char)ei].y;
                    
                    enemies[(unsigned char)ei].active = 0;
                    pending_kills++; // Increment kill counter
                    active_enemies_count--;
                    
                    // Large slime splits into 2 small slimes (keep this - it's gameplay critical)
                    if (enemy_type == 4) {
                      spawn_enemy(enemy_x - 8, enemy_y, 5);
                      spawn_enemy(enemy_x + 8, enemy_y, 5);
                    }
                    
                    // Queue pickup for later spawning (avoid expensive work in loop)
                    if (pending_pickup_count < 4) {
                        d = roll_drop_pickup();
                        if (d) {
                            pending_pickups[pending_pickup_count][0] = enemy_x + 4;
                            pending_pickups[pending_pickup_count][1] = enemy_y;
                            pending_pickups[pending_pickup_count][2] = d - 1; // Store type as 0 or 1
                            pending_pickup_count++;
                        }
                    }
                }

                break;
            }
            }
        }

        // Draw AFTER collision (skip if bullet died)
        if (bullets[bi].active) {
            oam_spr(bullets[bi].x, bullets[bi].y, BULLET_TILE, BULLET_PAL);
        }
    }
    
    // Process all deferred work AFTER collision loop
    if (pending_kills) {
        score_add(ENEMY_KILL_SCORE * pending_kills);
    }
    
    // Spawn all pending pickups
    for (bi = 0; bi < pending_pickup_count; bi++) {
        spawn_pickup(pending_pickups[bi][0], pending_pickups[bi][1], pending_pickups[bi][2]);
    }
}

void super_update_collide_draw(void) {
  signed char ei;
  unsigned char lane, l2;

  if (!superbullet.active) return;

  superbullet.y += superbullet.vy;
  superbullet.x += superbullet.vx;

  if (superbullet.y < 0x10) {
    superbullet.active = 0;
    return;
  }

  lane = (unsigned char)((superbullet.x + 8) >> LANE_SHIFT); // 16px wide -> +8 center 
  
  // Pre-calculate loop bounds to avoid expensive ternary and double-comparison
  {
      unsigned char lane_start = (lane > 0) ? (lane - 1) : 0;
      unsigned char lane_end = (lane < LANES - 1) ? (lane + 1) : (LANES - 1);
      
      for (l2 = lane_start; l2 <= lane_end; l2++) {
    ei = lane_enemy[l2];
    if (ei < 0) continue;
    
    // Add active check
    if (!enemies[(unsigned char)ei].active) continue;

    if (check_collision(&superbullet, &enemies[(unsigned char)ei])) {
      // kill enemy, but KEEP super active (pierce)
      unsigned char enemy_type = enemies[(unsigned char)ei].type;
      unsigned char enemy_x = enemies[(unsigned char)ei].x;
      unsigned char enemy_y = enemies[(unsigned char)ei].y;
      
      enemies[(unsigned char)ei].active = 0;
      active_enemies_count--;
      
      // Large slime splits into 2 small slimes
      if (enemy_type == 4) {
        spawn_enemy(enemy_x - 8, enemy_y, 5);  // Small slime left
        spawn_enemy(enemy_x + 8, enemy_y, 5);  // Small slime right
      }
      
      score_add(ENEMY_KILL_SCORE);
      break;
    }
      }
  }

  // draw 16x16 metasprite ONCE per frame
  oam_meta_spr(superbullet.x, superbullet.y, large_bullet);
}


void enemy_bullets_update_collide_draw(void) {
    unsigned char bi;

    for (bi = 0; bi < MAX_ENEMY_BULLETS; bi++) {
        if (!ebullets[bi].active) continue;

        ebullets[bi].y += ebullets[bi].vy;
        ebullets[bi].x += ebullets[bi].vx;

        if (ebullets[bi].y > 0xF0) {
            ebullets[bi].active = 0;
            continue;
        }
        if(ebullets[bi].x < 0x08 || ebullets[bi].x > 0xF8) {
            ebullets[bi].active = 0;
            continue;
        }
        
        // Collision check with player
        // Note: Inline invincibility check to avoid function call overhead
        if (check_collision((Sprite*)&ebullets[bi], (Sprite*)&wizard)) {
            ebullets[bi].active = 0;
            player_take_damage();  // This function handles invincibility internally
            continue;
        }

        // Draw: Golem heavy bullets use 16x16 metasprite, others use 4x4 sprite
        if (ebullets[bi].palette == 0xFF) {
            oam_meta_spr(ebullets[bi].x, ebullets[bi].y, golem_bullet);
        } else {
            oam_spr(ebullets[bi].x, ebullets[bi].y, BULLET_TILE, ebullets[bi].palette);
        }
    }
}
