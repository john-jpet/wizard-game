/* src/main.c
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
#include "pickup.h"
typedef enum {
  STATE_TITLE = 0,
  STATE_PLAY,
  STATE_GAMEOVER
} GameState;

static GameState state;
static unsigned char state_just_entered;

static unsigned char last_hp;
static unsigned int last_score;
static unsigned int last_mp;

static unsigned char hp;
static unsigned char mp;

static unsigned char framecounter;
static unsigned char wave;

Wave waves[] = {
  {3, 90, 1800, {0, 100, 0, 0, 0, 0, 0}},   // Wave 1
  {4, 90, 2400, {0, 70, 0, 30, 0, 0, 0}},  // Wave 2
  {4, 80, 2400, {25, 50, 0, 25, 0, 0, 0}},   // Wave 3
  {4, 60, 1800, {75, 25, 0, 0, 0, 0, 0}}, // Wave 4
  {4, 80, 2400, {40, 25, 10, 25, 0, 0, 0}},   // Wave 5
  {4, 70, 3600, {40, 0, 10, 20, 30, 0, 0}},  // Wave 6
  {4, 60, 4200, {0, 40, 0, 40, 0, 0, 20}}, // Wave 7
  {4, 80, 1800, {30, 0, 70, 0, 0, 0, 0}}, // Wave 8
  {5, 40, 4200, {0, 30, 0, 30, 30, 0, 10}}, // Wave 9
  {4, 70, 2400, {0, 5, 5, 15, 25, 0, 50}}  // Wave 10
};

// Store original durations for wave reset
const unsigned int wave_durations[] = {1800, 2400, 2400, 1800, 2400, 3600, 4200, 1800, 4200, 2400};

static unsigned char rand_range(unsigned char min, unsigned char max) {
  return (unsigned char)(min + (rand8() % (max - min + 1)));
}

static void set_state(GameState s) {
  state = s;
  state_just_entered = 1;
}
static void enter_title(void) {
  ppu_off();
  clear_vram_buffer();

  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);

  pal_bg(palette_bg);
  pal_spr(palette_sp);
  bank_spr(1);

  // draw title text with screen OFF (direct VRAM writes)
  vram_adr(NTADR_A(8, 10));
  vram_write("WIZARD'S STAND", 14);

  vram_adr(NTADR_A(6, 14));
  vram_write("PRESS START", 11);

  ppu_on_all();
}

static void enter_play(void) {
  unsigned char i;
  
  // Clear screen once (optional if you want HUD area preserved)
  ppu_off();
  clear_vram_buffer();
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);
  ppu_on_all();

  // reset systems
  bullets_init();
  ebullets_init();
  enemies_init();
  player_init();
  pickups_init();
  score_reset();
  music_play(0);
  
  // Reset wave durations (they get modified during gameplay)
  for (i = 0; i < 10; i++) {
    waves[i].duration = wave_durations[i];
  }

  // any counters
  //enemycounter = 0;
  framecounter = 0;
  wave = 0;

  // IMPORTANT: enable vram buffer updates during gameplay
  set_vram_buffer();
  clear_vram_buffer();
  hud_init();
  hud_set_wave(1); // display wave number (1-based)
  last_hp = 255;     // force HUD redraw
  last_mp = 255;     // force HUD redraw
  last_score = 0xFFFFFFFF;
}

static void enter_gameover(void) {
  // queue the text via vram buffer (screen is ON in gameplay)
  clear_vram_buffer();
  multi_vram_buffer_horz("GAME OVER", 9, NTADR_A(10, 14));
  multi_vram_buffer_horz("PRESS START", 11, NTADR_A(8, 16));
}
static void update_title(void) {
  unsigned char p = pad_poll(0);
  if (p & PAD_START) {
    seed_rng();
    set_state(STATE_PLAY);
  }
}

static void update_play(void) {
  player_update();

  //enemycounter++;

  waves[wave].duration--;
  framecounter++;
  if (waves[wave].duration == 0) {
    wave++;
    hud_set_wave(wave + 1); // display wave number (1-based)
    if (wave >= 11) wave = 9; // stay on last wave
    framecounter = 0;
  }
  // Spawn enemies at different intervals

  if (framecounter >= waves[wave].spawn_interval) {
    // Check if we're already at max enemies for this wave
    if (count_active_enemies() < waves[wave].max_enemies) {
      unsigned char r;
      unsigned char i;
      unsigned char enemy_type = 0;

      

      // Roll a random number between 0 and total_weight - 1
      r = rand8() % 100; // Assuming weights sum to 100

      // Determine enemy type based on weights
      for (i = 0; i < 7; i++) {
        if (r < waves[wave].enemy_weights[i]) {
          enemy_type = i;
          break;
        }
        r -= waves[wave].enemy_weights[i];
      }

      spawn_enemy(rand_range(16, 224), 0x10, enemy_type);  // Reduced max from 240 to 224 to prevent lane overflow
    }
    framecounter = 0;
  }


  enemies_update_and_draw();
  build_lane_enemy_table();

  player_bullets_update_collide_draw();
  super_update_collide_draw();
  enemy_bullets_update_collide_draw();
  pickups_update_draw();
  nuke_update_fx();

  // HUD update (only when dirty)
  hp = player_get_hp();
  if(hp != last_hp) {
    last_hp = hp;
    hud_set_hp(hp);
  }
  if(score != last_score) {
    last_score = score;
    hud_set_score(score);
  }
  mp = player_get_mp();
  if(mp != last_mp) {
    last_mp = mp;
    hud_set_mp(mp);
  }

  if (player_is_dead()) {
    set_state(STATE_GAMEOVER);
  }
}

static void update_gameover(void) {
  unsigned char p = pad_poll(0);
  if (p & PAD_START) {
    set_state(STATE_PLAY);
  }
}

static void draw_play(void) {
  player_draw();
}




static void boot_screen(void) {
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

  // ensure vblank after the ppu_off/ppu_on work
  ppu_wait_nmi();
}

void main(void) {

  boot_screen();
  set_state(STATE_TITLE);

  while (1) {
    ppu_wait_nmi();
    oam_clear();

    // Always start each frame with a clean buffer.
    // (You can remove this later if you’re careful to not overflow.)
    clear_vram_buffer();
	
	if(state_just_entered) {
	  state_just_entered = 0;
	  if(state == STATE_TITLE) enter_title();
	  else if(state == STATE_PLAY) enter_play(); 
	  else if(state == STATE_GAMEOVER) enter_gameover();
	}

    if (state == STATE_TITLE) {
      update_title();
    } else if (state == STATE_PLAY) {
      update_play();
      draw_play();
    } else { // STATE_GAMEOVER
      update_gameover();
    }
  }
}
