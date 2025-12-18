/*
 * Wizard's Stand: Defender of the Realm (NES)
 * -----------------
 * Author: John Petruzziello
 * Built with cc65 and neslib / nesdoug from Doug Fraker
 *
 * Description:
 * Core gameplay logic, enemy systems, and collision handling.
 */
 

// Include neslib and nesdoug headers
#include "neslib.h"
#include "nesdoug.h"


// Definitions

// max number of bullets/enemies on screen
#define MAX_BULLETS 4
#define MAX_ENEMIES 6

// bullet properties
#define BULLET_TILE 0x15
#define BULLET_PAL 1

// enemy properties
#define ENEMY_PAL 2
#define ENEMY_SPAWN_MIN_X  8
#define ENEMY_SPAWN_MAX_X  232   // 256 - 16 - 8

// cheap broad-phase before calling check_collision
#define BULLET_NEAR_X  20
#define BULLET_NEAR_Y  20

// hit detection macro
#define LANES 16
signed char lane_enemy[LANES];   // index of best candidate enemy per lane (-1 = none)
#define LANE_SHIFT 4  // 16px lanes

// open zeropage for frequently used variables
#pragma bss-name(push, "ZEROPAGE")

// Input variables
unsigned char pad;
unsigned char pad_prev;
unsigned char pad_new;

// Movement and animation variables
unsigned char moving = 0;
unsigned char right = 0;

unsigned char animframe = 0;
unsigned char animtimer = 0;

// Loop counters
unsigned char enemy_i;
unsigned char bullet_i;
unsigned char ebullet_i;

// Game variables
unsigned char enemycounter = 0;
unsigned char collision;

// End zeropage
#pragma bss-name(pop)

// Struct definitions

// Sprite structure
struct sprite {
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
};

// Player and star sprites
struct sprite wizard = {0x8F,0xCC,15,15};
struct sprite star = {0x10,0x10,7,7};

// Enemy structure
struct enemy {
    unsigned char x, y, width, height;
    signed char vx, vy;
    unsigned char hp;
    unsigned char active;
    unsigned char anim;      // optional
	unsigned char type;		 // optional
};

// Bullet structure
struct fireball {
    unsigned char x, y, width, height;
    signed char vx, vy;         // signed = can go left/up
    unsigned char active;
};

// Enemy bullet structure
struct enemyfireball {
    unsigned char x, y, width, height;
    signed char vx, vy;         // signed = can go left/up
    unsigned char active;
};

// Arrays for enemy bullets, enemies, and player bullets
struct enemyfireball ebullets[MAX_BULLETS];
struct enemy enemies[MAX_ENEMIES];
struct fireball bullets[MAX_BULLETS];

// Sprite data
const unsigned char wizardwalkr2[]={
	  0,  0,0x20,0|OAM_FLIP_H,
	  0,  8,0x30,0|OAM_FLIP_H,
	  8,  0,0x21,0|OAM_FLIP_H,
	  8,  8,0x31,0|OAM_FLIP_H,
	128
};

const unsigned char wizardwalkr1[]={
	  0,  0,0x02,0|OAM_FLIP_H,
	  0,  8,0x12,0|OAM_FLIP_H,
	  8,  0,0x03,0|OAM_FLIP_H,
	  8,  8,0x13,0|OAM_FLIP_H,
	128
};

const unsigned char wizardwalk2[]={
	  0,  0,0x21,0,
	  0,  8,0x31,0,
	  8,  0,0x20,0,
	  8,  8,0x30,0,
	128
};

const unsigned char wizardwalk1[]={
	  0,  0,0x03,0,
	  0,  8,0x13,0,
	  8,  0,0x02,0,
	  8,  8,0x12,0,
	128
};

const unsigned char wizardstand[]={
	  0,  0,0x01,0,
	  0,  8,0x10,0|OAM_FLIP_H,
	  8,  0,0x00,0,
	  8,  8,0x10,0,
	128
};

const unsigned char wizardfire[]={
	  0,  0,0x05,0,
	  0,  8,0x14,0|OAM_FLIP_H,
	  8,  0,0x04,0,
	  8,  8,0x14,0,
	128
};

const unsigned char imp[]={
	  0,  0,0x22,2|OAM_FLIP_H,
	  0,  8,0x32,2|OAM_FLIP_H,
	  8,  0,0x22,2,
	  8,  8,0x32,2,
	128
};

const unsigned char imp1[]={
	  0,  0,0x23,2|OAM_FLIP_H,
	  0,  8,0x33,2|OAM_FLIP_H,
	  8,  0,0x23,2,
	  8,  8,0x33,2,
	128
};
const unsigned char diveimp[]={
	  0,  0,0x22,1|OAM_FLIP_H,
	  0,  8,0x32,1|OAM_FLIP_H,
	  8,  0,0x22,1,
	  8,  8,0x32,1,
	128
};

const unsigned char diveimp1[]={
	  0,  0,0x23,1|OAM_FLIP_H,
	  0,  8,0x33,1|OAM_FLIP_H,
	  8,  0,0x23,1,
	  8,  8,0x33,1,
	128
};

// Palette data
const unsigned char palette_bg[]={
0x0f, 0x00, 0x10, 0x30, // black, gray, lt gray, white
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 

const unsigned char palette_sp[]={
0x0f, 0x02, 0x27, 0x36, // black, dark red, red, apricot
0x0f, 0x06, 0x16, 0x27, // black, dk blue, red, yellow
0x0f,0x03,0x13,0x23,
0,0,0,0
}; 



// Text data
const unsigned char text[]="Press any button to start!";

// Function prototypes

// Utility functions

// Check enemy lane based on enemy x position
    //Parameters: ex - enemy x position
static unsigned char enemy_lane_from_center(unsigned char ex) {
    // enemy is 16px wide -> center = x + 8
    return (unsigned char)((ex + 8) >> LANE_SHIFT);
}

// Check bullet lane based on bullet x position
    //Parameters: bx - bullet x position
static unsigned char bullet_lane_from_center(unsigned char bx) {
    // bullet is 4px wide -> center = x + 2
    return (unsigned char)((bx + 2) >> LANE_SHIFT);
}

// Random range function
    //Parameters: min - minimum value, max - maximum value
unsigned char rand_range(unsigned char min, unsigned char max) {
    // inclusive range [min, max]
    return min + (rand8() % (max - min + 1));
}

// Bullet initialization
    // Initializes all player bullets to inactive
void bullets_init(void) {
    for (bullet_i = 0; bullet_i < MAX_BULLETS; bullet_i++) {
        bullets[bullet_i].active = 0;
    }
}

// Fire a player bullet
    //Parameters: x - x position, y - y position, vx - x velocity, vy - y velocity
void fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy) {
    for (bullet_i = 0; bullet_i < MAX_BULLETS; bullet_i++) {
        if (!bullets[bullet_i].active) {
            bullets[bullet_i].active = 1;
            bullets[bullet_i].x = x;
            bullets[bullet_i].y = y;
			bullets[bullet_i].width = 4;
			bullets[bullet_i].height = 4;
            bullets[bullet_i].vx = vx;
            bullets[bullet_i].vy = vy;
            return;
        }
    }
}

// Enemy bullet initialization
    // Initializes all enemy bullets to inactive
void ebullets_init(void) {
    for (ebullet_i = 0; ebullet_i < MAX_BULLETS; ebullet_i++) {
        ebullets[ebullet_i].active = 0;
    }
}

// Fire an enemy bullet
    //Parameters: x - x position, y - y position, vx - x velocity, vy - y velocity
void enemy_fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy) {
    for (ebullet_i = 0; ebullet_i < MAX_BULLETS; ebullet_i++) {
        if (!ebullets[ebullet_i].active) {
            ebullets[ebullet_i].active = 1;
            ebullets[ebullet_i].x = x;
            ebullets[ebullet_i].y = y;
			ebullets[ebullet_i].width = 4;
			ebullets[ebullet_i].height = 4;
            ebullets[ebullet_i].vx = vx;
            ebullets[ebullet_i].vy = vy;
            return;
        }
    }
}

// Enemy initialization
    // Initializes all enemies to inactive
void enemies_init(void) {
    for (enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
        enemies[enemy_i].active = 0;
    }
}

// Spawn a new enemy
    //Parameters: x - x position, y - y position, type - enemy type
void spawn_enemy(unsigned char x, unsigned char y, unsigned char type) {
    for (enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
        if (!enemies[enemy_i].active) {
            enemies[enemy_i].active = 1;
            enemies[enemy_i].x = x;
            enemies[enemy_i].y = y;
			enemies[enemy_i].width = 16;
			enemies[enemy_i].height = 16;
            enemies[enemy_i].vx = 1;
            enemies[enemy_i].vy = 0;
            enemies[enemy_i].hp = (type == 0) ? 3 : 1;
            enemies[enemy_i].anim = rand8() & 63;
			enemies[enemy_i].type = type;
            return;
        }
    }
}

// Update enemy positions and behaviors
void enemies_update() {
    for (enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {

        if (!enemies[enemy_i].active) continue;
		if(enemies[enemy_i].type == 0) enemies[enemy_i].y++;
		else if (enemies[enemy_i].type == 1) enemies[enemy_i].y+=2;

        enemies[enemy_i].anim++;
		if(enemies[enemy_i].anim >= 60) enemies[enemy_i].anim = 0;
		if(enemies[enemy_i].anim == 30 && enemies[enemy_i].type == 0) {
			enemy_fire_bullet(enemies[enemy_i].x + 4, enemies[enemy_i].y + 8, 0, ENEMY_PAL);
		}	
    }
}

// Draw enemies on screen
    // Uses different sprites based on animation frame and type
void enemies_draw(void) {
    for (enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
        if (!enemies[enemy_i].active) continue;

		if(enemies[enemy_i].anim < 30)
			if(enemies[enemy_i].type == 0)
				oam_meta_spr(enemies[enemy_i].x, enemies[enemy_i].y, imp);
			else
				oam_meta_spr(enemies[enemy_i].x, enemies[enemy_i].y, diveimp);
		else
			if(enemies[enemy_i].type == 0)
				oam_meta_spr(enemies[enemy_i].x, enemies[enemy_i].y, imp1);
			else
				oam_meta_spr(enemies[enemy_i].x, enemies[enemy_i].y, diveimp1);
		//	
    }
}

// Animate the wizard based on movement and actions
    // Updates animation frames and draws the appropriate sprite
void animate_wizard(void) {
	if (moving){
			animtimer++;
			if (animtimer == 4){
				animtimer = 0;
				animframe++;
				
			}
			if (animframe & 1){
				if(right & 1) {
					oam_meta_spr(wizard.x, wizard.y, wizardwalkr1);
				} else {
					oam_meta_spr(wizard.x, wizard.y, wizardwalk1);
				}
				
			} else {
				if(right & 1) {
					oam_meta_spr(wizard.x, wizard.y, wizardwalkr2);
				} else {
					oam_meta_spr(wizard.x, wizard.y, wizardwalk2);
				}
			}
		} else {
			animtimer = 0;
			animframe = 0;
			if(pad & PAD_B) {
				oam_meta_spr(wizard.x, wizard.y, wizardfire);
			} else
			oam_meta_spr(wizard.x, wizard.y, wizardstand);
		}
}

// Build lane to enemy mapping for collision detection
void build_lane_enemy_table(void) {
    unsigned char l;

    for (l = 0; l < LANES; l++) lane_enemy[l] = -1;

    for (enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
        if (!enemies[enemy_i].active) continue;

        l = enemy_lane_from_center(enemies[enemy_i].x);

        // choose the enemy that is "closest to the player" (largest y)
        if (lane_enemy[l] < 0 || enemies[enemy_i].y > enemies[(unsigned char)lane_enemy[l]].y) {
            lane_enemy[l] = (signed char)enemy_i;
        }
    }
}

// Check collision between a bullet and an enemy
void player_bullets_update_collide_draw(void) {
    for (bullet_i = 0; bullet_i < MAX_BULLETS; bullet_i++) {
        signed char ei;
        unsigned char lane;
        unsigned char l2;

        if (!bullets[bullet_i].active) continue;

        // update
        bullets[bullet_i].y += bullets[bullet_i].vy;

        // offscreen
        if (bullets[bullet_i].y < 0x10) {
            bullets[bullet_i].active = 0;
            continue;
        }

        lane = bullet_lane_from_center(bullets[bullet_i].x);

        // Check lane-1, lane, lane+1 (boundary-safe)
        for (l2 = (lane ? lane - 1 : lane); l2 <= lane + 1 && l2 < LANES; l2++) {

            ei = lane_enemy[l2];
            if (ei < 0) continue;

            // precise check against that candidate
            if (check_collision(&bullets[bullet_i], &enemies[(unsigned char)ei])) {
                bullets[bullet_i].active = 0;

                if (enemies[(unsigned char)ei].hp) enemies[(unsigned char)ei].hp--;
                if (enemies[(unsigned char)ei].hp == 0) enemies[(unsigned char)ei].active = 0;

                break; // bullet is gone
            }
        }

        if (bullets[bullet_i].active) {
            oam_spr(bullets[bullet_i].x, bullets[bullet_i].y, BULLET_TILE, BULLET_PAL);
        }
    }
}



// Main game loop
void main (void) {
	
	ppu_off(); // screen off
	bullets_init();
	enemies_init();
	ebullets_init();
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_sp);

	// use the second set of tiles for sprites
	// both bg and sprite are set to 0 by default
	bank_spr(1);

	// load the text
	// vram_adr(NTADR_A(x,y));
	vram_adr(NTADR_A(1,1)); // set a start position for the text
	
	// vram_write draws the array to the screen
	vram_write(text,sizeof(text));
	
	ppu_on_all(); // turn on screen

    // Wait for player to press a button
	while (1) {
		ppu_wait_nmi();
		pad = pad_poll(0);
		if (pad) break;
	}

	// seed RNG using frame count at this moment
	seed_rng();
	
	//Erase the text currently on the screen
	vram_adr(NTADR_A(1,1)); // set a start position for the text
	vram_fill(0x00,sizeof(text)); // fill with spaces (0x00)
	
    // Main game loop
	while (1){

		ppu_wait_nmi(); 
		
		// infinite loop

		// wait till beginning of the frame
		// the sprites are pushed from a buffer to the OAM during nmi
		
		// clear all sprites from sprite buffer
		oam_clear();
		
		// push a single sprite
		// oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr);
		// use tile #0, palette #0
		//oam_meta_spr(wizard.x, wizard.y, wizardstand);
		//oam_spr(star.x, star.y, 0x11, 1);
		
		pad = pad_poll(0);

		// bits that are 1 now, but were 0 last frame
		pad_new = (pad ^ pad_prev) & pad;

		pad_prev = pad;

		moving = 0;
		if (pad_new & PAD_B) {
			fire_bullet(wizard.x + 4, wizard.y + 8, 0, -4);
		}
		for(ebullet_i = 0; ebullet_i < MAX_BULLETS; ebullet_i++) {
			if (ebullets[ebullet_i].active) {
				// update bullet position
				ebullets[ebullet_i].x += ebullets[ebullet_i].vx;
				ebullets[ebullet_i].y += ebullets[ebullet_i].vy;
				// draw the bullet
				oam_spr(ebullets[ebullet_i].x, ebullets[ebullet_i].y, BULLET_TILE, ENEMY_PAL);
				// deactivate if offscreen
				if (ebullets[ebullet_i].y > 0xE0) {
					ebullets[ebullet_i].active = 0;
				}
			}
		}
		if (pad & PAD_LEFT)  {wizard.x-=2; moving=1; right = 0;}
		if (pad & PAD_RIGHT) {wizard.x+=2; moving=1; right = 1;}
        enemycounter++;
		if (enemycounter == 240)
		{
			enemycounter = 0;
			spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 0);
		}
		if(enemycounter == 120) {
			spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 1);
		}
		enemies_update();
		build_lane_enemy_table();          // NEW (after enemy movement)
		enemies_draw();
		animate_wizard();
		player_bullets_update_collide_draw();
        
		//if (pad & PAD_UP)    y_position--;
		//if (pad & PAD_DOWN)  y_position++;
		
	}
}
	
	