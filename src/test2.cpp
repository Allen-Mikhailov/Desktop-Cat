#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>

#pragma comment(lib, "Dwmapi.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASSA wc = { sizeof(WNDCLASSA) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
    wc.lpszClassName = "MyWindowClass";

    if (!RegisterClassA(&wc)) {
        printf("Class register failed");
        return -1;
    }

    DWORD windowStyle = WS_OVERLAPPED ;
    // 

    // Create the window
    HWND hWnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST, "MyWindowClass", "My Window",
        WS_DLGFRAME  & ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        printf("Window creation fail");
        return -1;
    }

    // Make the window transparent (optional)
    SetWindowLong(hWnd, GWL_EXSTYLE, (GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT));
    SetLayeredWindowAttributes(hWnd, RGB(255,255, 255), 0, LWA_COLORKEY);

    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

    SetWindowPos(hWnd, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    // MARGINS margins = { -1 };
    // DwmExtendFrameIntoClientArea(hWnd, &margins);

    // Show the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SetMenu(hWnd, NULL);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // Perform your drawing operations here
            // For example, draw a red rectangle
            RECT rect = { 50, 50, 200, 200 };
            FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 0, 0)));
            EndPaint(hWnd, &ps);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
