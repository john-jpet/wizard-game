@echo off
setlocal
cd /d "%~dp0\.."

set name=wizard

REM ---- point these at your cc65 install ----
REM If tools\ is inside the cc65 folder, CC65_HOME=..\ is fine.
REM Otherwise set CC65_HOME to your real cc65 root:
REM set CC65_HOME=C:\Users\johnp\Documents\CC65\cc65-snapshot-win32
set CC65_HOME=..\

set PATH=%PATH%;%CC65_HOME%\bin

if not exist build mkdir build

REM ---- C -> .s ----
cc65 -Oirs -I include src\main.c    --add-source -o build\main.s
cc65 -Oirs -I include src\player.c  --add-source -o build\player.s
cc65 -Oirs -I include src\enemies.c --add-source -o build\enemies.s
cc65 -Oirs -I include src\bullets.c --add-source -o build\bullets.s
cc65 -Oirs -I include src\gfx.c     --add-source -o build\gfx.s
cc65 -Oirs -I include src\hud.c --add-source -o build\hud.s
cc65 -Oirs -I include src\game.c   --add-source -o build\game.s
cc65 -Oirs -I include src\pickup.c --add-source -o build\pickup.s

REM ---- assemble ----
ca65 lib\crt0.s        -o build\crt0.o
ca65 build\main.s   -g -o build\main.o
ca65 build\player.s -g -o build\player.o
ca65 build\enemies.s -g -o build\enemies.o
ca65 build\bullets.s -g -o build\bullets.o
ca65 build\gfx.s    -g -o build\gfx.o
ca65 build\hud.s -g -o build\hud.o
ca65 build\game.s   -g -o build\game.o
ca65 build\pickup.s -g -o build\pickup.o
ca65 assets\music\SoundFx.s -o build\SoundFx.o

REM ---- link ----
ld65 -C cfg\nrom_32k_vert.cfg ^
  -o build\%name%.nes ^
  build\crt0.o build\main.o build\player.o build\enemies.o build\bullets.o build\gfx.o build\hud.o build\game.o build\pickup.o build\SoundFx.o ^
  "%CC65_HOME%\lib\nes.lib" ^
  -Ln build\labels.txt --dbgfile build\dbg.txt -m build\%name%.map

pause
