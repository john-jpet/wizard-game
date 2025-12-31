# Wizard's Stand - Defender of the Realm (NES Homebrew)

A simple NES homebrew arcade survival game: move a wizard left/right, shoot upward, survive descending enemies and their projectiles.

## Gameplay
- You control a wizard at the bottom of the screen.
- Enemies descend from above; some shoot projectiles downward.
- Shoot enemies with fireballs to survive and score points.

## Controls
- D-Pad Left/Right: move
- B: fire
- A: super
- Start/Any button: start game (also used to seed RNG)

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
