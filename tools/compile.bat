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

REM ---- assemble ----
ca65 lib\crt0.s        -o build\crt0.o
ca65 build\main.s   -g -o build\main.o
ca65 build\player.s -g -o build\player.o
ca65 build\enemies.s -g -o build\enemies.o
ca65 build\bullets.s -g -o build\bullets.o
ca65 build\gfx.s    -g -o build\gfx.o

REM ---- link ----
ld65 -C cfg\nrom_32k_vert.cfg ^
  -o build\%name%.nes ^
  build\crt0.o build\main.o build\player.o build\enemies.o build\bullets.o build\gfx.o ^
  "%CC65_HOME%\lib\nes.lib" ^
  -Ln build\labels.txt --dbgfile build\dbg.txt

pause
