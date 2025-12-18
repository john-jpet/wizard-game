// src/player.c
#include "player.h"
#include "bullets.h"
#include "neslib.h"
#include "nesdoug.h"
#include "gfx.h"


// Player sprite state
Sprite wizard = { 0x8F, 0xCC, 16, 16 };

// Input + animation state lives here (player-owned)
static unsigned char pad;
static unsigned char pad_prev;
static unsigned char pad_new;

static unsigned char moving;
static unsigned char right;
static unsigned char animframe;
static unsigned char animtimer;

static void animate_wizard(void) {
    if (moving) {
        animtimer++;
        if (animtimer == 4) {
            animtimer = 0;
            animframe++;
        }

        if (animframe & 1) {
            oam_meta_spr(wizard.x, wizard.y, (right ? wizardwalkr1 : wizardwalk1));
        } else {
            oam_meta_spr(wizard.x, wizard.y, (right ? wizardwalkr2 : wizardwalk2));
        }
    } else {
        animtimer = 0;
        animframe = 0;
        if (pad & PAD_B) {
            oam_meta_spr(wizard.x, wizard.y, wizardfire);
        } else {
            oam_meta_spr(wizard.x, wizard.y, wizardstand);
        }
    }
}

void player_init(void) {
    // If you want: reset everything cleanly
    pad = pad_prev = pad_new = 0;
    moving = right = 0;
    animframe = animtimer = 0;

    wizard.x = 0x8F;
    wizard.y = 0xCC;
    wizard.width = 16;
    wizard.height = 16;
}

void player_update(void) {
    pad = pad_poll(0);
    pad_new = (pad ^ pad_prev) & pad;
    pad_prev = pad;

    moving = 0;

    // Fire bullet on new press
    if (pad_new & PAD_B) {
        fire_bullet(wizard.x + 4, wizard.y + 8, 0, -4);
    }

    // Movement
    if (pad & PAD_LEFT)  { wizard.x -= 2; moving = 1; right = 0; }
    if (pad & PAD_RIGHT) { wizard.x += 2; moving = 1; right = 1; }

    // Potential future change?: clamp to screen bounds
    // if (wizard.x < 8) wizard.x = 8;
    // if (wizard.x > 232) wizard.x = 232;
}

void player_draw(void) {
    animate_wizard();
}
