@REM @echo off
g++ main.c glad.c -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17 -static -static-libgcc -static-libstdc++
a.exe