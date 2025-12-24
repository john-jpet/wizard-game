// src/player.c
#include "player.h"
#include "bullets.h"
#include "neslib.h"
#include "nesdoug.h"

Sprite wizard = { 0x8F, 0xCC, 16, 16 };

static unsigned char pad, pad_prev, pad_new;
static unsigned char moving, right, animframe, animtimer;

// health state
static unsigned char hp;
static unsigned char mp;
static unsigned char iframes;   // counts down when >0

extern const unsigned char wizardwalkr1[];
extern const unsigned char wizardwalkr2[];
extern const unsigned char wizardwalk1[];
extern const unsigned char wizardwalk2[];
extern const unsigned char wizardstand[];
extern const unsigned char wizardfire[];

static void animate_wizard(void) {
    if (moving) {
        animtimer++;
        if (animtimer == 4) { animtimer = 0; animframe++; }

        if (animframe & 1) {
            oam_meta_spr(wizard.x, wizard.y, (right ? wizardwalkr1 : wizardwalk1));
        } else {
            oam_meta_spr(wizard.x, wizard.y, (right ? wizardwalkr2 : wizardwalk2));
        }
    } else {
        animtimer = 0;
        animframe = 0;
        if (pad & PAD_B || pad & PAD_A) oam_meta_spr(wizard.x, wizard.y, wizardfire);
        else            oam_meta_spr(wizard.x, wizard.y, wizardstand);
    }
}

void player_init(void) {
    pad = pad_prev = pad_new = 0;
    moving = right = 0;
    animframe = animtimer = 0;

    wizard.x = 0x8F;
    wizard.y = 0xCC;
    wizard.width = 16;
    wizard.height = 16;

    hp = 3;
    mp = 0;
    iframes = 0;
}

void player_update(void) {
    if (iframes) iframes--;

    pad = pad_poll(0);
    pad_new = (pad ^ pad_prev) & pad;
    pad_prev = pad;

    moving = 0;

    if (pad_new & PAD_B) {
        fire_bullet(wizard.x + 4, wizard.y + 8, 0, -4);
    }
    if(pad_new & PAD_A) {
        if(mp >= 1) {
            fire_super(wizard.x, wizard.y, 0, -2);
        }
    }

    if (pad & PAD_LEFT)  { wizard.x -= 2; moving = 1; right = 0; }
    if (pad & PAD_RIGHT) { wizard.x += 2; moving = 1; right = 1; }

    // if (wizard.x < 8) wizard.x = 8;
    // if (wizard.x > 232) wizard.x = 232;
}

void player_draw(void) {
    // blink while invincible (skip drawing on alternating frames)
    if (iframes) {
        if (iframes & 2) return; // adjust mask for blink speed
    }
    animate_wizard();
}

unsigned char player_get_hp(void) { return hp; }
unsigned char player_get_mp(void) { return mp; }
unsigned char player_is_dead(void) { return (hp == 0); }
unsigned char player_is_invincible(void) { return (iframes != 0); }

void player_take_damage(void) {
    if (hp == 0) return;
    if (iframes) return;

    hp--;
    iframes = 60; // ~1 second at 60fps
}

void player_heal(void) {
    if (hp < 3) {
        hp++;
    }
}

void player_add_mp(unsigned char amount) {
    mp += amount;
    if (mp > 5) mp = 5; // max mp
}

void player_use_mp(unsigned char amount) {
    if (mp >= amount) {
        mp -= amount;
    }
}