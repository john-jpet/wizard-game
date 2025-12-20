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
#include "hud.h"

typedef enum {
  STATE_PLAY = 0,
  STATE_GAMEOVER = 1
} GameState;

static GameState state;

static unsigned char enemycounter;
static unsigned char last_hp;
static unsigned char last_score;

static unsigned char rand_range(unsigned char min, unsigned char max) {
  return (unsigned char)(min + (rand8() % (max - min + 1)));
}

/*static void draw_hud_hp(unsigned char hp) {
  // "HP:" at (1,1) and digit at (4,1)
  one_vram_buffer('H', NTADR_A(1, 1));
  one_vram_buffer('P', NTADR_A(2, 1));
  one_vram_buffer(':', NTADR_A(3, 1));
  one_vram_buffer((unsigned char)('0' + hp), NTADR_A(4, 1));
}*/

static void show_game_over(void) {
  // Only call once on transition into GAMEOVER
  multi_vram_buffer_horz("GAME OVER", 9, NTADR_A(10, 14));
}

static void reset_gameplay(void) {
  bullets_init();
  ebullets_init();
  enemies_init();
  player_init();
  score_reset();
  hud_init();

  enemycounter = 0;
  last_hp = 255;     // force HUD redraw
  state = STATE_PLAY;

  // Clear the screen once at reset (optional but nice)
  ppu_off();
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);
  ppu_on_all();

  // Make sure buffer is active/empty
  set_vram_buffer();
  clear_vram_buffer();
}

static void boot_screen(void) {
  unsigned char p;

  ppu_off();
  set_vram_buffer();
  clear_vram_buffer();

  // clear nametable at boot
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);

  // graphics setup
  pal_bg(palette_bg);
  pal_spr(palette_sp);
  bank_spr(1);

  ppu_on_all();

  // wait for any input
  while (1) {
    ppu_wait_nmi();
    p = pad_poll(0);
    if (p) break;
  }

  seed_rng();

  reset_gameplay();

  // ensure vblank after the ppu_off/ppu_on work
  ppu_wait_nmi();
}

void main(void) {
  unsigned char hp;
  unsigned char p;

  boot_screen();

  while (1) {
    ppu_wait_nmi();
    oam_clear();

    // Always start each frame with a clean buffer.
    // (You can remove this later if you’re careful to not overflow.)
    clear_vram_buffer();

    if (state == STATE_PLAY) {

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

      player_bullets_update_collide_draw();
      enemy_bullets_update_collide_draw();

      // -------- HUD (only if changed) --------
      hp = player_get_hp();
      if (hp != last_hp) {
        hud_set_hp(hp);
		hud_set_mp(4); // for testing
        last_hp = hp;
      }
	  if (score != last_score) {
		hud_set_score(score);
		last_score = score;
	  }

      // -------- DRAW --------
      enemies_draw();
      player_draw();

      if (player_is_dead()) {
        show_game_over();
        state = STATE_GAMEOVER;
      }

    } else { // STATE_GAMEOVER

      // keep showing last frame sprites? (optional)
      enemies_draw();
      player_draw();

      // restart on START
      p = pad_poll(0);
      if (p & PAD_START) {
        reset_gameplay();
      }
    }
  }
}
