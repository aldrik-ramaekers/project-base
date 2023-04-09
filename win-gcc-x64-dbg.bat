DEL /S /Q build >nul
mkdir build

gcc -m64 -c -Wall -Wno-maybe-uninitialized -DMODE_DEBUG -DLIBPROJECTBASE_IMPLEMENTATION -g src/entry.c -o build/libprojectbase-debug.o
ar rcs build/libprojectbase-debug.a build/libprojectbase-debug.o

robocopy -a /IS /IT /IM /MIR "src/." C:/mingw64/x86_64-w64-mingw32/include/projectbase > nul
robocopy /IS /IT /IM /MIR "build/" "C:/mingw64/x86_64-w64-mingw32/lib/" libprojectbase-debug.a > nul

robocopy /IS /IT /IM /MIR -a "examples/data/." "build/data" > nul
gcc -m64 -g -DMODE_DEBUG examples/example_window.c -o build/example_window.exe

IF "%1"=="r" call "build/example_window.exe"