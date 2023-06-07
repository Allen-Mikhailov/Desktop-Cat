#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <dwmapi.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <time.h>

int running = 1;

clock_t startT;
clock_t lastT;

HDC hdc;
HDC hdcMem;
BITMAP bitmap;
HGDIOBJ oldBitmap;
HINSTANCE hinstance;

COLORREF TRANSPARENT_COLOR = RGB(255,255,255);
HBRUSH transparentBrush = (HBRUSH) CreateSolidBrush(RGB(255, 0, 0));

// Window vars
int client_width;
int client_height;

HBITMAP catSpriteMap;

// Cat Variables
double catX = 0;
float catY = 0;
int catDir = 0;
int catAnim = 0;

char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

const int SPRITE_SCALE = 2;
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

    // top
    RECT rect = {catX-SPRITE_UNIT, catY-SPRITE_UNIT, catX+SPRITE_UNIT*2, catY};
    FillRect(hdc, &rect, transparentBrush);
}

void paint(HWND window)
{
    POINT p;
    GetCursorPos(&p);

    float xDiff = (p.x-catX);
    float yDiff = (p.y-catY);
    float c = hypot(p.x-catX, p.y-catY);

    if (c != 0)
    {
        if (xDiff != 0)
            catX += min(abs(xDiff)/c*5, abs(xDiff)) * (xDiff/abs(xDiff));
        if (yDiff != 0)
            catY += min(abs(yDiff)/c*5, abs(yDiff)) * (yDiff/abs(yDiff));
    }

    double elapsed_time = ( (double)clock() - (double)startT)/CLOCKS_PER_SEC;
    double delta_time = ( (double)clock() - (double)lastT)/CLOCKS_PER_SEC;
    lastT = clock();

    DrawCat((int) catX, (int) catY, 5, 6, (int) (elapsed_time/.1)%8);

}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;

    static PAINTSTRUCT ps;

    switch (message)
    {
    case WM_CREATE:
        // SetTimer(window, 1, 20, NULL); 
        hdcMem = CreateCompatibleDC(hdc);
        catSpriteMap = (HBITMAP) LoadImageA(hinstance, "./Cats2x.bmp", IMAGE_BITMAP, 2048, 1088, LR_LOADFROMFILE);

        oldBitmap = SelectObject(hdcMem, catSpriteMap);
        GetObject(catSpriteMap, sizeof(bitmap), &bitmap);
        break;
    case WM_CLOSE:
        running = 0;
        break;
    // case WM_ERASEBKGND:
    //     return 1;

    case WM_TIMER:
        // SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
        InvalidateRect(window, NULL, FALSE);
        break;
    case WM_KEYDOWN:
        
        break;
    case WM_PAINT:
        paint(window);
        break;

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

    HWND window;

    PIXELFORMATDESCRIPTOR pfd;

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

    // memory Buffer

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