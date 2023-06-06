#include <windows.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// #define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glad/gl.h>

#include <stdio.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    int mWidth  = mi.rcMonitor.right - mi.rcMonitor.left;
    int mHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(mWidth, mHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("Terminate");
        glfwTerminate();
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    HWND hWnd = glfwGetWin32Window(window);

    // Transparent
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);

    LONG cur_style = GetWindowLong(hWnd, GWL_EXSTYLE);
    SetWindowLong(hWnd, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_OVERLAPPED | WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_DISABLED | WS_EX_TOOLWINDOW);

    // Top most
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}