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
void destroy(HWND window);

void paint(HWND window, HDC paintHDC)
{
    double elapsed_time = ( (double)clock() - (double)startT)/CLOCKS_PER_SEC;
    double delta_time = ( (double)clock() - (double)lastT)/CLOCKS_PER_SEC;
    lastT = clock();

    update(delta_time);
    frames++;
}

void error_check(const char* string)
{
    DWORD lastError = GetLastError();

    // Buffer to hold the formatted message
    LPWSTR errorMsg = nullptr;

    // Call FormatMessageW to retrieve the error message
    DWORD dwSize = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        lastError,
        0, // Default language
        reinterpret_cast<LPWSTR>(&errorMsg),
        0,
        nullptr
    );

    if (dwSize == 0) {
        // Handle the error if FormatMessageW fails
        DWORD dwError = GetLastError();
        wprintf(L"FormatMessageW failed with error %lu\n", dwError);
    } else {
        // Print or use the retrieved error message
        wprintf(L"Error %lu:\n", lastError);
        wprintf(L"%ls\n", errorMsg);

        // Don't forget to free the allocated buffer
        LocalFree(errorMsg);
    }

    SetLastError(0);
    
    // if (lastError != 0)
    //     printf("%s failed with error code %lu\n", string, lastError);
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
    switch (message)
    {
    case WM_SYSCOMMAND:
        if (w_param == SC_CLOSE)
        {
            // Handle close button click in the taskbar
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

    hdc = GetDC(window);

    // Transparent
    SetLayeredWindowAttributes(window, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Starting dt clocks
    startT = clock();
    lastT = clock();

    init(window, hdc);
    error_check("init error");
    
    MSG message;
    while (running && GetMessage(&message, window, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_PAINT)
        {
            Sleep(5);
        }
    }

    // Clean up
    ReleaseDC(window, hdc);
    DestroyWindow(window);
    KillTimer(window, 1);
    DeleteObject(transparentBrush);

    destroy(window);

    return 0;
}