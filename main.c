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

    const COLORREF rgbRed   =  0x000000FF;
    FillRect(hdc, &prect, (HBRUSH) (CreateSolidBrush(rgbRed)));

    // HFONT hFont; 
    // hFont = (HFONT)GetStockObject(ANSI_VAR_FONT); 
    // TextOut(hdc, 120, 120, "Sample ANSI_VAR_FONT text", 25); 

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

    HWND window;

    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "Sample Window Class";

    RegisterClassA(&window_class);

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);


    window = CreateWindowEx(0,
                            "Sample Window Class",
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


    // Idk anymore
    SetWindowPos(window, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    LONG cur_style = GetWindowLong(window, GWL_EXSTYLE);
    SetWindowLong(window, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);

    void* images = malloc(sizeof(byte) * 8 * 6 * 6 * 32 * 32);

    char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
    char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};


    FILE *fptr;

    for (int anim = 0; anim < 6; anim++)
    {
        for (int dir = 0; dir < 8; dir++)
        {
            char filename[] = "./CatAnims/FF-FF.32x32frames";

            // Anim Name
            filename[11] = animations[anim][0];
            filename[12] = animations[anim][1];


            // Direction name
            filename[14] = directions[dir][0];
            filename[15] = directions[dir][1];

            // atoi();

            fptr = fopen(filename, "r");

            char i[1];
            char* ip = i;

            fgets(ip, 1, fptr);

            printf(ip);
        }
    }

    // fptr = fopen("filename.txt", "r");

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