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

g++ "src\cat.cpp" -lgdi32 -lstdc++ -ldwmapi -o"build\Desktop Cat.exe"

@REM Adding the rest of the files
copy src\Cats.bmp build

echo.
echo Running................................................
echo.

"build\Desktop Cat.exe"

echo.
echo Finished Running................................................
echo.

set /p id=Press "Enter" to exit