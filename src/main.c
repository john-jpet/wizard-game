/*
 * Wizard's Stand: Defender of the Realm (NES)
 * Author: John Petruzziello
 *
 * Main loop + high-level orchestration only.
 */

#include "game.h"
#include "player.h"
#include "enemies.h"
#include "bullets.h"
#include "gfx.h"


static unsigned char enemycounter;

static unsigned char rand_range(unsigned char min, unsigned char max) {
  return min + (rand8() % (max - min + 1));
}

static void boot_screen(void) {
  unsigned char p;

  ppu_off();
  set_vram_buffer();
  clear_vram_buffer();

  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);

  // init systems
  bullets_init();
  ebullets_init();
  enemies_init();
  player_init();

  // graphics setup
  pal_bg(palette_bg);
  pal_spr(palette_sp);
  bank_spr(1);

  // TEXT TEMPORARILY DISABLED
//   vram_adr(NTADR_A(1,1));
//   vram_write(text, sizeof(text));
	

  ppu_on_all();

  // wait for any input
  while (1) {
    ppu_wait_nmi();
    p = pad_poll(0);
    if (p) break;
  }

  // seed RNG using frame count at this moment
  
  seed_rng();

//   TEXT TEMPORARILY DISABLED
//   vram_adr(NTADR_A(1,1));
//   vram_fill(0x00, sizeof(text));

  enemycounter = 0;
  ppu_wait_nmi();                 // ensure vblank
  
}
static unsigned char game_over_shown = 0;

static void show_game_over(void) {
  clear_vram_buffer();                 // wipe queued updates for this frame
  one_vram_buffer('0', NTADR_A(4,1));
  multi_vram_buffer_horz("GAME OVER", 9, NTADR_A(10, 14));
  game_over_shown = 1;
}

void main(void) {
  boot_screen();
  

  while (1) {
    ppu_wait_nmi();
	clear_vram_buffer();    // IMPORTANT: prevents “stuck” writes

	one_vram_buffer('H', NTADR_A(1,1));
	one_vram_buffer('P', NTADR_A(2,1));
	one_vram_buffer(':', NTADR_A(3,1));
	one_vram_buffer('0' + player_get_hp(), NTADR_A(4,1));
	if (player_is_dead()) {
		
		if (!game_over_shown) show_game_over();

		// Optional: stop gameplay updates, but keep drawing the player/enemies if you want
		// If you want a hard freeze, just loop waiting for NMI:
		while (1) { ppu_wait_nmi(); }
    }
    oam_clear();
	


    // -------- UPDATE --------
    player_update();

    enemycounter++;
    if (enemycounter == 120) {
      spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 1);
    }
    if (enemycounter == 240) {
      enemycounter = 0;
      spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 0);
    }

    enemies_update();
    build_lane_enemy_table();

    // bullets handle update + collision + draw internally
    player_bullets_update_collide_draw();
    enemy_bullets_update_collide_draw();

    // -------- DRAW --------
    enemies_draw();
    player_draw();
  }
}
