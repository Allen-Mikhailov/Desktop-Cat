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
#include "config.cpp"

#include "extra_math.cpp"

int running = 1;

clock_t startT;
clock_t lastT;

COLORREF TRANSPARENT_COLOR = RGB(255,255,255);
HBRUSH transparentBrush = (HBRUSH) CreateSolidBrush(TRANSPARENT_COLOR);

HDC hdc;
HINSTANCE hinstance;
HDC paintHDC;

// Window vars
int client_width;
int client_height;

int frames = 0;

void update(double delta_time);
void init(HWND window, HDC hdc);
void destroy();

void paint(HWND window, HDC paintHDC)
{
    double elapsed_time = ( (double)clock() - (double)startT)/CLOCKS_PER_SEC;
    double delta_time = ( (double)clock() - (double)lastT)/CLOCKS_PER_SEC;
    lastT = clock();

    // PAINTSTRUCT ps;
    // ps.rcPaint = {0, 0, client_width, client_height};
    // paintHDC = BeginPaint(window, &ps);
    

    update(delta_time);

    // EndPaint(window, &ps);
    frames++;
}

void error_check(const char* string)
{
    DWORD lastError = GetLastError();
    if (lastError != 0)
        printf("%s failed with error code %lu\n", string, lastError);
}

void close_window()
{
    running = 0;
    PostQuitMessage(0);
}

RECT getScreenRect()
{
    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    return mi.rcMonitor;
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;

    static PAINTSTRUCT ps;
    // printf("Message%d\n", message);

    switch (message)
    {
    case WM_SYSCOMMAND:
        if (w_param == SC_CLOSE)
        {
            // Handle close button click in the taskbar
            printf("Task quit\n");
            close_window();
        }
        else
        {
            result = DefWindowProc(window, message, w_param, l_param);
        }
        break;
    case WM_QUIT:
        DestroyWindow(window);
        return 0;
    // case WM_PAINT:
    //     printf("WTF");
    //     break;

    // case WM_TIMER:
    //     if (w_param == 1)
    //         paint(window, hdc);
    //         // SendMessage(window, WM_PAINT, NULL, NULL);
    //     return 0;
    case WM_PAINT:
        paint(window, hdc);
        return 0;
    default:
        result = DefWindowProc(window,
                               message,
                               w_param,
                               l_param);
        break;  
    }

    return result;
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
    window_class.lpszClassName = className;

    hinstance = instance;

    RegisterClassA(&window_class);

    error_check("register");

    RECT screen_rect = getScreenRect();
    client_width = getRectWidth(screen_rect);
    client_height = getRectHeight(screen_rect);

    DWORD styles = WS_EX_LAYERED | WS_EX_TRANSPARENT;
    int flags = WS_OVERLAPPED | WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_DISABLED | WS_EX_TOOLWINDOW;
    window = CreateWindowExA(styles,
                            className,
                            windowName,
                            flags,
                            screen_rect.left,
                            screen_rect.top,
                            client_width,
                            client_height,
                            0,
                            0,
                            instance,
                            0);

    error_check("window create");

    hdc = GetDC(window);

    error_check("hdc");

    // Transparent
    // SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(window, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    error_check("transparecnt");

    // Starting dt clocks
    startT = clock();
    lastT = clock();
    
    // SetTimer(window, 1, 1, NULL); 

    error_check("clock/timer error");

    init(window, hdc);
    error_check("init error");
    
    MSG message;
    while (GetMessage(&message, window, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        // // printf("message %d\n", message.message);
        if (message.message == WM_PAINT)
        {
            Sleep(5);
        }
    }

    ReleaseDC(window, hdc);
    DestroyWindow(window);

    KillTimer(window, 1);

    return 0;
}