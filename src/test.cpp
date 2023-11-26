#include <windows.h>
#include <cstdio>

int x = 0;

// Function to handle WM_PAINT message
void OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // Perform your drawing operations here
    // For simplicity, let's just draw a rectangle
    Rectangle(hdc, x, 50, x+150, 200);

    EndPaint(hWnd, &ps);
    x++;
}

// Function to handle the WM_TIMER message
void OnTimer(HWND hWnd) {
    // Trigger a repaint on each timer tick
    InvalidateRect(hWnd, NULL, TRUE);
}

// Main window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT:
            OnPaint(hWnd);
            return 0;

        case WM_TIMER:
            OnTimer(hWnd);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = "MyWindowClass";

    if (!RegisterClassA(&wc)) {
        DWORD error = GetLastError();
        printf("Fail to rekister %lu\n", error);
        return -1;
    }

    // Create the window
    HWND hWnd = CreateWindowExW(0,L"MyWindowClass", L"My Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        printf("window fail");
        return -1;
    }

    // Set up a timer with a 16-millisecond interval (for approximately 60 frames per second)
    SetTimer(hWnd, 1, 16, NULL);

    // Show and update the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up the timer before exiting
    KillTimer(hWnd, 1);

    return (int)msg.wParam;
}
