
#include "pickup.h"
#include "game.h"
#include "player.h"

Pickup pickups[MAX_PICKUPS];

void pickups_init(void) {
  unsigned char i;
  for (i = 0; i < MAX_PICKUPS; i++) {
    pickups[i].active = 0;
  }
}

void spawn_pickup(unsigned char x, unsigned char y, unsigned char type) {
  unsigned char i;
  for (i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active) {
      pickups[i].active = 1;
      pickups[i].x = x;
      pickups[i].y = y;
      pickups[i].width = 8;
      pickups[i].height = 8;
      pickups[i].type = type;
      return;
    }
  }
}

unsigned char tile;
void pickups_update_draw(void) {
  unsigned char i;
  for (i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active) continue;

    pickups[i].y += 1;
    if (pickups[i].y > 0xF0) {
      pickups[i].active = 0;
      continue;
    }
    if(check_collision((Sprite*)&pickups[i], (Sprite*)&wizard)) {
      pickups[i].active = 0;
      if(pickups[i].type == 0) {
        player_heal();
      } else if(pickups[i].type == 1) {
        player_add_mp(1);
        score_add(500);
      }
      continue;
    }

    tile = (pickups[i].type == 0) ? HEART_PICKUP_TILE : STAR_PICKUP_TILE;
    oam_spr(pickups[i].x, pickups[i].y, tile, PICKUPS_PAL);
  }
}