
#ifndef PICKUP_H
#define PICKUP_H

#define HEART_PICKUP_TILE 0x40
#define STAR_PICKUP_TILE 0x11
#define MAX_PICKUPS 5
#define PICKUPS_PAL 1

typedef struct {
  unsigned char x;
  unsigned char y;
  unsigned char width;
  unsigned char height;
  unsigned char active;
  unsigned char type;  // 0 = heart, 1 = star
} Pickup;

extern Pickup pickups[MAX_PICKUPS];
void pickups_init(void);
void spawn_pickup(unsigned char x, unsigned char y, unsigned char type);
void pickups_update_draw(void);

#endif