// src/bullets.c
#include "bullets.h"
#include "enemies.h"
#include "player.h"

Fireball bullets[MAX_BULLETS];
EnemyFireball ebullets[MAX_BULLETS];

void bullets_init(void) {
    unsigned char i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
}

void ebullets_init(void) {
    unsigned char i;
    for (i = 0; i < MAX_BULLETS; i++) {
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
            return;
        }
    }
}

void enemy_fire_bullet(unsigned char x, unsigned char y, signed char vx, signed char vy) {
    unsigned char i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!ebullets[i].active) {
            ebullets[i].active = 1;
            ebullets[i].x = x;
            ebullets[i].y = y;
            ebullets[i].width = 4;
            ebullets[i].height = 4;
            ebullets[i].vx = vx;
            ebullets[i].vy = vy;
            return;
        }
    }
}

static unsigned char bullet_lane_from_center(unsigned char bx) {
    return (unsigned char)((bx + 4) >> LANE_SHIFT);
}

void player_bullets_update_collide_draw(void) {
    unsigned char bi;
    signed char ei;
    unsigned char lane;
    unsigned char l2;

    for (bi = 0; bi < MAX_BULLETS; bi++) {

        if (!bullets[bi].active) continue;

        bullets[bi].y += bullets[bi].vy;

        if (bullets[bi].y < 0x10) {
            bullets[bi].active = 0;
            continue;
        }

        lane = bullet_lane_from_center(bullets[bi].x);

        for (l2 = (lane ? (unsigned char)(lane - 1) : lane);
             l2 <= (unsigned char)(lane + 1) && l2 < LANES;
             l2++) {

            ei = lane_enemy[l2];
            if (ei < 0) continue;

            if (check_collision((Sprite*)&bullets[bi], (Sprite*)&enemies[(unsigned char)ei])) {
                bullets[bi].active = 0;

                if (enemies[(unsigned char)ei].hp) enemies[(unsigned char)ei].hp--;
                if (enemies[(unsigned char)ei].hp == 0) enemies[(unsigned char)ei].active = 0;

                break;
            }
        }

        if (bullets[bi].active) {
            oam_spr(bullets[bi].x, bullets[bi].y, BULLET_TILE, BULLET_PAL);
        }
    }
}

void enemy_bullets_update_collide_draw(void) {
    unsigned char bi;

    for (bi = 0; bi < MAX_BULLETS; bi++) {
        if (!ebullets[bi].active) continue;

        ebullets[bi].y += ebullets[bi].vy;

        if (ebullets[bi].y > 0xF0) {
            ebullets[bi].active = 0;
            continue;
        }
        
        // collide with player
        if (!player_is_invincible() && check_collision(&ebullets[bi], &wizard)) {
            ebullets[bi].active = 0;
            player_take_damage();
            continue;
        }

        oam_spr(ebullets[bi].x, ebullets[bi].y, BULLET_TILE, ENEMY_PAL);
    }
}
