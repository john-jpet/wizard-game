# Wizard's Stand - Defender of the Realm (NES Homebrew)

A wave-based arcade survival game for the NES. Control a wizard defending the realm against increasingly difficult waves of enemies with unique behaviors and abilities.

## Gameplay
- You control a wizard at the bottom of the screen
- Enemies descend from above in 10 progressive waves
- Shoot enemies with fireballs, dodge projectiles, and survive
- Collect hearts (HP) and stars (MP) from defeated enemies
- Use special abilities: super bullets (piercing) and screen-clearing nukes

## Enemy Types

### Type 0: Imp
- **HP:** 2
- **Speed:** 0.5 px/frame (moderate)
- **Behavior:** Fires single bullets downward at regular intervals
- **Threat:** Steady bullet pressure, requires consistent dodging

### Type 1: Speed Demon
- **HP:** 1
- **Speed:** 1.0 px/frame (fast)
- **Behavior:** Dives quickly, no projectiles
- **Threat:** Speed makes positioning critical, easy to kill if hit

### Type 2: Warlock
- **HP:** 2
- **Speed:** 0.2 px/frame (slow), stops when firing
- **Behavior:** Fires 3-bullet spread pattern, long firing animation
- **Threat:** Spread pattern covers wide area, multiple warlocks create bullet hell

### Type 3: Fire Spirit
- **HP:** 1
- **Speed:** 0.33 px/frame vertical, 1.0 px/frame horizontal weaving
- **Behavior:** Weaves left-right while descending, no projectiles
- **Threat:** Unpredictable movement makes timing shots difficult

### Type 4: Large Slime
- **HP:** 3
- **Speed:** 0.2 px/frame (slow)
- **Behavior:** Splits into 2 Small Slimes (Type 5) when destroyed
- **Threat:** Creates additional enemies, can overwhelm if not managed

### Type 5: Small Slime
- **HP:** 1
- **Speed:** 1.0 px/frame (fast)
- **Behavior:** Only spawns from Large Slime splits, no projectiles
- **Threat:** Sudden appearance from splits can surprise players

### Type 6: Golem (Berserker Tank)
- **HP:** 7 (highest in game)
- **Speed:** 0.1 → 0.5 px/frame (accelerates when damaged)
- **Behavior:** 
  - Starts very slow (0.1 px/frame)
  - Speeds up dramatically with each hit
  - Fires large 16x16 heavy bullets at 6 HP and 3 HP
  - Heavy bullets travel at 3 px/frame (50% faster than normal)
- **Speed Progression:**
  - 7 HP: 0.10 px/frame (very slow)
  - 6 HP: 0.17 px/frame (+70%) → **Fires heavy bullet #1**
  - 5 HP: 0.25 px/frame (+47%)
  - 4 HP: 0.33 px/frame (+32%)
  - 3 HP: 0.50 px/frame (+50%, max speed) → **Fires heavy bullet #2**
- **Threat:** Punishes chip damage, rewards burst damage, heavy bullets force dodging which allows golem to advance at high speed

## Wave Progression

The game features 10 waves with increasing difficulty:
- **Wave 1-3:** Introduction to basic enemy types and bullet patterns
- **Wave 4:** Difficulty spike with faster spawning and denser enemies
- **Wave 5-6:** Introduction of advanced enemy combinations
- **Wave 7:** First golems appear as mini-boss threats
- **Wave 8:** Bullet hell wave with high warlock concentration
- **Wave 9:** Breather wave (no bullet-firing enemies, max 5 enemies)
- **Wave 10:** Final challenge with 50% golem spawns

Total gameplay time: ~7.5 minutes before boss (not yet implemented)

## Controls
- D-Pad Left/Right: move
- B: fire
- A: super
- Up + A: screen wipe
- Start: start game (also used to seed RNG)

## Build Requirements
- cc65 toolchain (cc65/ca65/ld65)
- A NES emulator (Mesen recommended)

Tested with: cc65 V2.19 (Git c720c3c)

## Build

### Build Artifacts
Compiled ROMs and build outputs are intentionally excluded from version control.

### Windows (batch script)
From the project root:
```bash
tools\compile.bat
