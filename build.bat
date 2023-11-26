@echo off

@REM Old Compile
@REM g++ restart.c -luser32 -lglu32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17 

echo.
echo Preparing Build Folder................................................
echo.

@REM Removing old build
del  /Q .\build\*

echo Building................................................
echo.

g++ "src\test2.cpp" -lgdi32 -lstdc++ -ldwmapi -o"build\Desktop Cat.exe"

@REM Adding the rest of the files
copy src\Cats.bmp build

cd build

echo.
echo Running................................................
echo.

"Desktop Cat.exe"

echo.
echo Finished Running................................................
echo.

cd ..