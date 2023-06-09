#include <windows.h>
#include <stdint.h>
#include <dwmapi.h>
#include <stdlib.h>

#include <string.h>

// #include <stdio.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

int running = 1;

clock_t startT;
clock_t lastT;

HDC hdc;
HDC hdcMem;
BITMAP bitmap;
HGDIOBJ oldBitmap;
HINSTANCE hinstance;

COLORREF TRANSPARENT_COLOR = RGB(255,255,255);
HBRUSH transparentBrush = (HBRUSH) CreateSolidBrush(TRANSPARENT_COLOR);

// Window vars
int client_width;
int client_height;

HBITMAP catSpriteMap;

// Cat Variables
int catAnimDir = 6;
int catAnim = 5;

double catAnimF = 0;

double catX = 500;
double catY = 500;

const int targetResetTime = 1000;
double catTargetOffsetX = 0, catTargetOffsetY = 0;

double catDirection = 0;
double catVelocity = 0;

const double catTurnSpeed = 3;
const double catVelocityCap = 1250;
const double catAcceleration = 750;

char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

const int SPRITE_SCALE = 3;
const int SPRITE_SIZE = 32;
const int SPRITE_UNIT = SPRITE_SCALE*SPRITE_SIZE;

using namespace std;

void DrawCat(int x, int y, int anim, int dir, int frame)
{
    // Drawing the cat and stuff
    int mapX = anim*4 + frame%4;
    int mapY = dir*2 + frame/4 + 1;
    BitBlt(hdc, x, y, SPRITE_UNIT, SPRITE_UNIT, hdcMem, SPRITE_UNIT*mapX, SPRITE_UNIT*mapY, SRCCOPY);

    // Clearing Area around the cat

    // Top
    RECT rect;
    rect = {(int) catX-SPRITE_UNIT, (int)catY-SPRITE_UNIT, (int)catX+SPRITE_UNIT*2, (int)catY};
    FillRect(hdc, &rect, transparentBrush);

    // Bottom
    rect = {(int)catX-SPRITE_UNIT, (int)catY+SPRITE_UNIT, (int)catX+SPRITE_UNIT*2, (int)catY+SPRITE_UNIT*2};
    FillRect(hdc, &rect, transparentBrush);

    // Left
    rect = {(int) catX-SPRITE_UNIT, (int)catY, (int)catX, (int)catY+SPRITE_UNIT};
    FillRect(hdc, &rect, transparentBrush);

    rect = {(int) catX+SPRITE_UNIT, (int)catY, (int)catX+SPRITE_UNIT*2, (int)catY+SPRITE_UNIT};
    FillRect(hdc, &rect, transparentBrush);
}

double sign(double v)
{
    if (v == 0)
        return v;
    return v / abs(v);
}

double realisticQuadratic(double a, double b, double c)
{
    double r1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
    double r2 = (-b - sqrt(b*b - 4*a*c))/(2*a);

    if (r1 <= 0)
        return r2;
    return r1;
}

void paint(HWND window)
{
    double elapsed_time = ( (double)clock() - (double)startT)/CLOCKS_PER_SEC;
    double delta_time = ( (double)clock() - (double)lastT)/CLOCKS_PER_SEC;
    lastT = clock();

    POINT p;
    GetCursorPos(&p);

    int targetX = p.x; //catTargetOffsetX;
    int targetY = p.y; //catTargetOffsetY;

    double xDiff = (targetX-catX-SPRITE_UNIT/2);
    double yDiff = (targetY-catY-SPRITE_UNIT/2);
    double c = hypot(xDiff, yDiff);

    // Direction
    double targetDir = fmod(M_PI*2 + atan2(-yDiff, xDiff), M_PI*2); // degrees
    double rotDif = targetDir - catDirection;
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
        // printf("%f\n", estimatedTime);
        // if (catVelocity)


        double dif = min(delta_time*catVelocity, c);
        if (xDiff != 0)
            catX += cos(catDirection)*dif;
        if (yDiff != 0)
            catY += -sin(catDirection)*dif;

        // printf("%d : %d\n", (int) dir, (int) catDir);

    }

    catAnimF = fmod(catAnimF+delta_time*catVelocity/catVelocityCap*50, 8);

    DrawCat((int) catX, (int) catY, catAnim, catAnimDir, (int) (catAnimF)%8);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;

    static PAINTSTRUCT ps;

    switch (message)
    {
    case WM_CREATE:
        SetTimer(window, 1, 1, NULL); 
        SetTimer(window, 2, targetResetTime, NULL); 
        hdcMem = CreateCompatibleDC(hdc);

        // 1x 1024 544
        // 2x 2048, 1088
        // 3x 3072, 1632
        catSpriteMap = (HBITMAP) LoadImageA(hinstance, "./Cats.bmp", IMAGE_BITMAP, 
            1024*SPRITE_SCALE, 544*SPRITE_SCALE, LR_LOADFROMFILE);

        oldBitmap = SelectObject(hdcMem, catSpriteMap);
        GetObject(catSpriteMap, sizeof(bitmap), &bitmap);
        break;
    case WM_CLOSE:
        running = 0;
        break;

    case WM_TIMER:
        if (w_param == 1)
            SendMessage(window, WM_PAINT, NULL, NULL);
        else if (w_param == 2)
        {
            // int offset = 300;
            catTargetOffsetX = (int) ((double)rand()/RAND_MAX*client_width);
            catTargetOffsetY = (int) ((double)rand()/RAND_MAX*client_height);
        }
        // if (w_param == 1)
        //     printf("Timer2");
        break;
    case WM_KEYDOWN:
        
        break;
    case WM_PAINT:
        paint(window);
        return DefWindowProc(window, message, w_param, l_param);

    default:
        result = DefWindowProc(window,
                               message,
                               w_param,
                               l_param);
        break;  
    }

    return result;
}

int intfromHex(const char* str)
{
    return (int)strtol(str, NULL, 16);
}

int APIENTRY WinMain(HINSTANCE instance,
                     HINSTANCE prev_instance,
                     LPSTR cmd_line,
                     int cmd_show)
{

    srand(time(NULL));

    HWND window;

    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "Sample Window Class";
    // window_class.hbrBackground	= NULL;

    hinstance = instance;

    RegisterClassA(&window_class);

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    client_width = mi.rcMonitor.right - mi.rcMonitor.left;
    client_height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    int flags = WS_OVERLAPPED | WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_DISABLED | WS_EX_TOOLWINDOW;

    window = CreateWindowExA(0,
                            "Sample Window Class",
                            "Game",
                            flags,
                            mi.rcMonitor.left,
                            mi.rcMonitor.top,
                            client_width,
                            client_height,
                            0,
                            0,
                            instance,
                            0);
    hdc = GetDC(window);

    // Transparent
    SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(window, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    startT = clock();
    lastT = clock();
    
    while (running)
    {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            // printf("Message");
            TranslateMessage(&message);
            DispatchMessage(&message);
            Sleep(1);
        }
    }

    ReleaseDC(window, hdc);
    DestroyWindow(window);

    return 0;
}