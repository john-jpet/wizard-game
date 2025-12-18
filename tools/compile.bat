@echo off
setlocal

REM run from repo root
cd /d "%~dp0\.."

set name=wizard

REM cc65 toolchain
if "%CC65_HOME%"=="" (
  echo CC65_HOME not set
  pause
  exit /b 1
)
set PATH=%PATH%;%CC65_HOME%\bin\

REM ensure build folder exists
if not exist build mkdir build

REM compile C -> .s (assembly)
cc65 --target nes -Oirs -I include src\main.c --add-source -o build\%name%.s

REM assemble startup + game
ca65 --target nes lib\crt0.s -o build\crt0.o
ca65 --target nes build\%name%.s -g -o build\%name%.o

REM link (IMPORTANT: use the toolchain nes.lib)
ld65 -C cfg\nrom_32k_vert.cfg ^
  -o build\%name%.nes ^
  build\crt0.o build\%name%.o "%CC65_HOME%\lib\nes.lib" ^
  -Ln build\labels.txt --dbgfile build\dbg.txt

pause
