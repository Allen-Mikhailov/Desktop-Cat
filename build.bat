@REM @echo off
g++ test.c glad.h -luser32 -lglew32 -lglfw3 -lglu32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17 
a.exe