@REM @echo off
g++ text.c  -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17 -static -static-libgcc -static-libstdc++
a.exe