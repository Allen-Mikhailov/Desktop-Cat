#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <dwmapi.h>

#include "libs/guibase.h"
// #include <wchar.h>

#ifdef u32
typedef uint32_t u32;
#endif

typedef char string[];

int running = 1;
int client_width = 640;
int client_height = 640;

HDC hdc;

void paint(HWND window)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(window, &ps);

    RECT prect = {0, 0, 100, 100};

    const COLORREF rgbWhite   =  0x00FFFFFF;

    FillRect(hdc, &prect, (HBRUSH) (CreateSolidBrush(rgbWhite)));

    const COLORREF rgbRed   =  0x000000FF;
    FillRect(hdc, &prect, (HBRUSH) (CreateSolidBrush(rgbRed)));

    HFONT hFont, hOldFont; 

    // Retrieve a handle to the variable stock font.  
    hFont = (HFONT)GetStockObject(ANSI_VAR_FONT); 

    TextOut(hdc, 120, 120, "Sample ANSI_VAR_FONT text", 25); 

    EndPaint(window, &ps);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;
    switch (message)
    {
    case WM_CLOSE:
        running = 0;
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

int APIENTRY WinMain(HINSTANCE instance,
                     HINSTANCE prev_instance,
                     LPSTR cmd_line,
                     int cmd_show)
{
    flipfont(basefont);
    // presentfont(basefont);

    HWND window;

    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "Game_Window_Class";

    if (!RegisterClassA(&window_class))
    {
        return 0;
    }

    RECT window_rect = {0, 0, client_width, client_height};

    AdjustWindowRectEx(&window_rect,
                       WS_OVERLAPPEDWINDOW,
                       0, 0);

    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    int window_x = (screen_width / 2) - (window_width / 2);
    int window_y = (screen_height / 2) - (window_height / 2);

    RegisterClass(&window_class);

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);


    window = CreateWindowEx(0,
                            window_class.lpszClassName,
                            "Game",
                            WS_OVERLAPPED  | WS_VISIBLE | WS_POPUP | WS_DISABLED & ~(WS_EX_APPWINDOW),
                            mi.rcMonitor.left,
                            mi.rcMonitor.top,
                            mi.rcMonitor.right - mi.rcMonitor.left,
                            mi.rcMonitor.bottom - mi.rcMonitor.top,
                            0,
                            0,
                            instance,
                            0);

    
    

    hdc = GetDC(window);

    // Transparent
    SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(window, RGB(255,255,255), 0, LWA_COLORKEY);

    // Removing Border
    LONG lExStyle = GetWindowLong(window, GWL_EXSTYLE);
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLongPtr(window, GWL_EXSTYLE, lExStyle);

    // Keeping window on top
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE 
        | SWP_NOSIZE 
        | WS_POPUPWINDOW
        );


    SetWindowPos(window, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    LONG cur_style = GetWindowLong(window, GWL_EXSTYLE);
    SetWindowLong(window, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);

    while (running)
    {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        BringWindowToTop(window);
    }

    return 0;
}