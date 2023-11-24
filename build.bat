@echo off

@REM Old Compile
@REM g++ restart.c -luser32 -lglu32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17 

echo Building................................................
echo.

g++ "src\cat.cpp" -lgdi32 -lstdc++ -o"build\Desktop Cat.exe"

echo.
echo transfering Cats.bmp................................................
echo.

copy src\Cats.bmp build

echo.
echo Running................................................
echo.

cd build
"Desktop Cat.exe"