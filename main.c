#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "libs/guibase.h"
// #include <wchar.h>

#ifdef u32
typedef uint32_t u32;
#endif

typedef char string[];

int running = 1;
int client_width = 640;
int client_height = 640;

void *memory;
BITMAPINFO bitmap_info;

render_buffer rb;

float target_seconds_per_frame = 1.0f / 30.0f;

LARGE_INTEGER frequency;

float get_seconds_per_frame(LARGE_INTEGER start_counter,
                            LARGE_INTEGER end_counter)
{
    return ((float)(end_counter.QuadPart - start_counter.QuadPart) / (float)frequency.QuadPart);
}

void clear_screen(u32 color)
{
    u32 *pixel = (u32 *) memory;

    for (int pixel_number = 0;
         pixel_number < client_width * client_height;
         ++pixel_number)
    {
        *pixel++ = color;
    }
}

void winrectupdate(HWND window)
{

    RECT rect;
    GetClientRect(window, &rect);
    client_width = rect.right - rect.left;
    client_height = rect.bottom - rect.top;

    rb.width = client_width;
    rb.height = client_height;

    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    }

    memory = VirtualAlloc(0,
                          client_width * client_height * 4,
                          MEM_RESERVE | MEM_COMMIT,
                          PAGE_READWRITE);

    rb.memory = memory;

    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result;
    switch (message)
    {
    case WM_SIZE:
        winrectupdate(window);
        break;
    case WM_CLOSE:
        running = 0;
        break;

    case WM_KEYDOWN:
        // switch (w_param)
        // {

        // }
        break;
    case WM_PAINT:
        printf("%d", RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
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
    // window_rect.left = 0;
    // window_rect.top = 0;
    // window_rect.right = client_width;
    // window_rect.bottom = client_height;

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

    window = CreateWindowEx(0,
                            window_class.lpszClassName,
                            "Game",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            window_x,
                            window_y,
                            window_width,
                            window_height,
                            0,
                            0,
                            instance,
                            0);

    HDC hdc = GetDC(window);

    // LARGE_INTEGER start_counter, end_counter, counts, fps, ms;

    // QueryPerformanceCounter(&start_counter);
    // QueryPerformanceFrequency(&frequency);

    // GUI_BASE testgui;
    // v2 testv2 = {0.0f, 0.0f};
    // testgui.anchorpoint.x = 0;
    // testgui.anchorpoint.y = 0;
    // testgui.size.x = 200;
    // testgui.size.y = 100;
    // testgui.position.x = 0;
    // testgui.position.y = 0;

    // string test = {7, 2, 6, 0, 1, 3, 5, 6};
    // testgui.textsize = 8;
    // testgui.text = &test;

    // rendergui(rb, testgui);

    while (running)
    {
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        //Frame cap
        // QueryPerformanceCounter(&end_counter);

        // float seconds_per_frame = get_seconds_per_frame(start_counter,
        //                                                 end_counter);

        // if (seconds_per_frame < target_seconds_per_frame)
        // {
        //     Sleep((DWORD)(1000 * (target_seconds_per_frame - seconds_per_frame)));

        //     while (seconds_per_frame < target_seconds_per_frame)
        //     {
        //         QueryPerformanceCounter(&end_counter);

        //         seconds_per_frame = get_seconds_per_frame(start_counter,
        //                                                   end_counter);
        //     }
        // }

        // QueryPerformanceCounter(&end_counter);

        // seconds_per_frame = get_seconds_per_frame(start_counter,
        //                                           end_counter);
        // start_counter = end_counter;

        // //Render
        // clear_screen(0x111111);

        // rendergui(rb, testgui);

        // //DrawBuffer
        // StretchDIBits(hdc,
        //               0,
        //               0,
        //               client_width,
        //               client_height,
        //               0,
        //               0,
        //               client_width,
        //               client_height,
        //               memory,
        //               &bitmap_info,
        //               DIB_RGB_COLORS,
        //               SRCCOPY);
    }

    return 0;
}