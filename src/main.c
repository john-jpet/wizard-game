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

// Game timing constants
#define ENEMY_SPAWN_FAST 120   // frames until fast enemy spawns
#define ENEMY_SPAWN_SLOW 240   // frames until slow enemy spawns
#define ENEMY_SPAWN_WARLOCK 360 // frames until warlock spawns (6 seconds)

typedef enum {
  STATE_TITLE = 0,
  STATE_PLAY,
  STATE_GAMEOVER
} GameState;

static GameState state;
static unsigned char state_just_entered;

static unsigned int enemycounter;  // Changed to unsigned int to reach 360
static unsigned char last_hp;
static unsigned int last_score;
static unsigned int last_mp;

static unsigned char hp;
static unsigned char mp;

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
  

  // any counters
  enemycounter = 0;

  // IMPORTANT: enable vram buffer updates during gameplay
  set_vram_buffer();
  clear_vram_buffer();
  hud_init();
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

  enemycounter++;
  
  // Spawn enemies at different intervals (check in order: smallest to largest)
  if (enemycounter == ENEMY_SPAWN_FAST) {
    spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 1);
  } 
  if (enemycounter == ENEMY_SPAWN_SLOW) {
    spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 0);
  }
  if (enemycounter >= ENEMY_SPAWN_WARLOCK) {
    spawn_enemy(rand_range(ENEMY_SPAWN_MIN_X, ENEMY_SPAWN_MAX_X), 0x10, 2);
    enemycounter = 0;  // Reset AFTER warlock spawns
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
