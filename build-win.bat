@echo off

windres misc/icon.rc -O coff -o misc/icon.res

DEL /S /Q bin
cd src

ld -r -b binary -o ../bin/data.o ../data/imgs/en.png  ../data/imgs/nl.png ../data/imgs/logo_64.png ../data/fonts/mono.ttf ../data/translations/en-English.mo ../data/translations/nl-Dutch.mo ../data/imgs/list.png ../data/imgs/delete.png ../data/imgs/exclaim.png ../data/imgs/add.png ../data/imgs/set.png

if "%1"=="-w" (SET defs=-DMODE_DEVELOPER -DMODE_GDBDEBUG) else (SET defs=-DMODE_DEVELOPER)

x86_64-w64-mingw32-gcc -m64 -Wall -g %defs% -Wno-unused-label -Wno-unused-variable mo_edit.c ../bin/data.o -o ../bin/moedit.exe ../misc/icon.res -lopengl32 -lkernel32 -lglu32 -lgdi32 -lcomdlg32 -lgdiplus -lole32 -lshlwapi

DEL /Q "../bin/data.o"

FOR %%A IN ("../bin/moedit.exe") DO set size=%%~zA
echo size = %size%

cd ../

if "%1"=="-r" start bin/moedit.exe
if "%1"=="-w" start gdb -ex run bin/moedit.exe