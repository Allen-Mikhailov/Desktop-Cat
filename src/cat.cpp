#include <windows.h>
#include <stdint.h>
#include <dwmapi.h>
#include <stdlib.h>

#include <string.h>

#include <stdio.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include "window_setup.cpp"

HDC catSheetHDC;
BITMAP bitmap;
HBITMAP catSpriteMap;

// Cat Variables
int catAnimDir = 6;
int catAnim = 5;

double catAnimF = 0;

double catX = 500;
double catY = 500;

double catVelocity = 0;

double catDirection = 0;

const int targetResetTime = 1000;
double catTargetOffsetX = 0, catTargetOffsetY = 0;

const double catTurnSpeed = 5;
const double catVelocityCap = 500;
const double catAcceleration = 1500;
const double FRICTION = .97;

char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

const int SPRITE_SCALE = 3;
const int SPRITE_SIZE = 32;
const int SPRITE_UNIT = SPRITE_SCALE*SPRITE_SIZE;

using namespace std;

const int BORDER_PADING = 200;
void changeCatTarget(int client_width, int client_height)
{
    catTargetOffsetX = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_width   - BORDER_PADING * 2 ) );
    catTargetOffsetY = BORDER_PADING + (int) ( (double) rand() / RAND_MAX* ( client_height  - BORDER_PADING * 2 ) );
}

HDC testHDC;
HBITMAP testMap;

HPEN hPen = CreatePen(PS_NULL, 1, RGB(255, 0, 0));

COLORREF RED = 0x000000FF;
HBRUSH redBrush = (HBRUSH) CreateSolidBrush(RED);

HBITMAP createCompatibleBitmap(HDC hdc, int width, int height)
{
    // Get the number of color planes
    int nPlanes = GetDeviceCaps(hdc, PLANES);

    // Get the number of bits per pixel
    int nBitCount = GetDeviceCaps(hdc, BITSPIXEL);

    const void* lpBits = malloc((((width * nPlanes * nBitCount + 15) >> 4) << 1) * height);
    return CreateBitmap(width, height, nPlanes, nBitCount, lpBits);
}

void init(HWND window, HDC hdc)
{
    SetTimer(window, 1, 1, NULL); 
    SetTimer(window, 2, targetResetTime, NULL); 

    SelectObject(hdc, hPen);

    catSheetHDC = CreateCompatibleDC(hdc);

    char filePath[MAX_PATH]; 
    DWORD filepath = GetModuleFileNameA(NULL, filePath, MAX_PATH);
    printf("Current Path is %s", filePath);

    // 1x 1024 544
    // 2x 2048, 1088
    // 3x 3072, 1632
    catSpriteMap = (HBITMAP) LoadImageA(hinstance, "./Cats.bmp", IMAGE_BITMAP, 
        1024*SPRITE_SCALE, 544*SPRITE_SCALE, LR_LOADFROMFILE);


    SelectObject(catSheetHDC, catSpriteMap);
    GetObject(catSpriteMap, sizeof(bitmap), &bitmap);

    


    changeCatTarget(client_width, client_height);

    testHDC = CreateCompatibleDC(catSheetHDC);
    testMap = createCompatibleBitmap(catSheetHDC, 200, 200);
    SelectObject(testHDC, testMap);
    // SelectObject(testHDC, redBrush);

    // SetBrushOrgEx(testHDC, 0, 0, NULL); // Reset brush origin
    // SetBrushOrgEx(testHDC, 0, 0, NULL); // Set brush origin to (0, 0)
    // SetBkMode(testHDC, OPAQUE);

    HBITMAP drawMap = testMap;
    HDC drawHDC = testHDC;

    // Drawing 1
    SelectObject(drawHDC, hPen);
    SelectObject(drawHDC, redBrush);
    Rectangle(drawHDC, 0, 0, 201, 201);

    // Drawing 2
    // RECT rect = {0, 0, 200, 200};
    // printf("stuff %d\n", FillRect(testHDC, &rect, redBrush));

    SelectObject(testHDC, testMap);

    PBITMAPINFO t = CreateBitmapInfoStruct(window, drawMap);
    LPTSTR str = (LPTSTR)"test.bmp";
    CreateBMPFile(window, str, t, drawMap, drawHDC);
}

void DrawCat(int x, int y, int anim, int dir, int frame, HDC hdc, HDC catSheetHDC)
{
    // Drawing the cat
    int mapX = anim*4 + frame%4;
    int mapY = dir*2 + frame/4 + 1;
    BitBlt(hdc, x, y, SPRITE_UNIT, SPRITE_UNIT, catSheetHDC, SPRITE_UNIT*mapX, SPRITE_UNIT*mapY, SRCCOPY);

    error_check("Cat draw");

    // Clearing Area around the cat
    SelectObject(hdc, hPen);

    error_check("pen select");

    // Top
    RECT rect;
    rect = {x-SPRITE_UNIT, y-SPRITE_UNIT, x+SPRITE_UNIT*2, y};
    FillRect(hdc, &rect, transparentBrush);
    // Rectangle(hdc, (int) catX-SPRITE_UNIT, (int)catY-SPRITE_UNIT, (int)catX+SPRITE_UNIT*2, (int)catY);

    // Bottom
    rect = {x-SPRITE_UNIT, y+SPRITE_UNIT, x+SPRITE_UNIT*2, y+SPRITE_UNIT*2};
    FillRect(hdc, &rect, transparentBrush);

    // Left
    rect = {x-SPRITE_UNIT, y, x, y+SPRITE_UNIT};
    FillRect(hdc, &rect, transparentBrush);

    rect = {x+SPRITE_UNIT, y, x+SPRITE_UNIT*2, y+SPRITE_UNIT};
    FillRect(hdc, &rect, transparentBrush);

    BitBlt(hdc, 0, 0, 5, 5, testHDC, 0, 0, SRCCOPY);
}

int getCatAnimDir(double x, double)
{
    return 0;
}

void update(double delta_time)
{
    POINT p;
    GetCursorPos(&p);

    int targetX;
    int targetY;

    // Mouse Targeting
    targetX = p.x;
    targetY = p.y;

    // Random Targeting
    // targetX = catTargetOffsetX;
    // targetY = catTargetOffsetY;

    double xDiff = (targetX-catX-SPRITE_UNIT/2);
    double yDiff = (targetY-catY-SPRITE_UNIT/2);
    double c = hypot(xDiff, yDiff);

    // Direction
    double targetDir = fmod(M_PI*2 + atan2(-yDiff, xDiff), M_PI*2);
    double rotDif = targetDir-catDirection;
    if (rotDif != 0)
    {
        double rotSign = sign(rotDif);
        if (abs(rotDif) > M_PI)
            rotSign *= -1;

        if (abs(catTurnSpeed*delta_time) > abs(rotDif))
            catDirection += rotDif;
        else 
            catDirection += rotSign*catTurnSpeed*delta_time;
        catDirection = fmod(M_PI*2 + catDirection, M_PI*2);


        catAnimDir = (int)((catDirection+M_PI/8) / (M_PI/4))%8;
        catAnimDir = (6 - catAnimDir + 8)%8;
    }

    // printf("%f, %f, %f\n", targetDir, catDirection, rotDif);

    if (c < 100)
        changeCatTarget(client_width, client_height);

    if (c != 0)
    {
        double estimatedTime = realisticQuadratic(
            -catAcceleration/2,
            catVelocity,
            -c
        );

        if (estimatedTime < catVelocity/catAcceleration)
            catVelocity =  max(catVelocity - catAcceleration * delta_time, 0.0);
        else
            catVelocity =  min(catVelocity + catAcceleration * delta_time, catVelocityCap);

        double dif = min(delta_time*catVelocity, c);
        if (xDiff != 0)
            catX += cos(catDirection)*dif;
        if (yDiff != 0)
            catY += -sin(catDirection)*dif;

        // printf("%d : %d\n", (int) dir, (int) catDir);

    }

    catAnimF = fmod(catAnimF+delta_time*catVelocity/catVelocityCap*50, 8);

    // To get rid of the black lines
    if (frames == 0)
    {
        RECT rect = {0, 0, client_width, client_height};
        FillRect(hdc, &rect, transparentBrush);
    }

    DrawCat((int) catX, (int) catY, catAnim, catAnimDir, (int) (catAnimF)%8, hdc, catSheetHDC);
}