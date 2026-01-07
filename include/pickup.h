// include/pickup.h
#ifndef PICKUP_H
#define PICKUP_H

#define HEART_PICKUP_TILE 0x40
#define STAR_PICKUP_TILE 0x11
#define MAX_PICKUPS 5
#define PICKUPS_PAL 1

#define DROP_HEART_CHANCE 8   // 1 in 8
#define DROP_STAR_CHANCE 16  // 1 in 16

typedef struct {
  unsigned char x;
  unsigned char y;
  unsigned char width;
  unsigned char height;
  unsigned char active;
  unsigned char type;  // 0 = heart, 1 = star
  unsigned char _padding[2];  // Pad to 8 bytes for fast array access
} Pickup;

extern Pickup pickups[MAX_PICKUPS];
void pickups_init(void);
void spawn_pickup(unsigned char x, unsigned char y, unsigned char type);
void pickups_update_draw(void);

#endif