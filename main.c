
/* An example of the minimal Win32 & OpenGL program.  It only works in
   16 bit color modes or higher (since it doesn't create a
   palette). */


#include <windows.h>			/* must include this before GL/gl.h */

#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>

#define GL_GLEXT_PROTOTYPES			/* OpenGL utilities header file */
#include <GL/glext.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int fileExists (char* name) {
    FILE *file;
    if((file = fopen(name,"r"))!=NULL)
    {
        // file exists
        fclose(file);
        return 1;
    }
    return 0;
}

void
display()
{
    /* rotate a triangle around */
    glClear(GL_COLOR_BUFFER_BIT);
    // glBegin(GL_TRIANGLES);
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glVertex2i(0,  1);
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glVertex2i(-1, -1);
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glVertex2i(1, -1);
    // glEnd();

    int width = 100;
    int height = 100;

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);
    glTexCoord2i(0, 1); glVertex2i(0, height);
    glTexCoord2i(1, 1); glVertex2i(width, height);
    glTexCoord2i(1, 0); glVertex2i(width, 0);
    glEnd();

    glFlush();

    // glutSwapBuffers();
}


LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    static PAINTSTRUCT ps;

    switch(uMsg) {
    case WM_PAINT:
        display();
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
	    return 0;

    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        PostMessage(hWnd, WM_PAINT, 0, 0);
	    return 0;

    case WM_CHAR:
	switch (wParam) {
	case 27:			/* ESC key */
	    PostQuitMessage(0);
	    break;
	}
	return 0;

    case WM_CLOSE:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

HWND
CreateOpenGLWindow(char* title, int x, int y, int width, int height, 
		   BYTE type, DWORD flags)
{
    int         pf;
    HDC         hDC;
    HWND        hWnd;
    WNDCLASS    wc;
    PIXELFORMATDESCRIPTOR pfd;
    static HINSTANCE hInstance = 0;

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
	hInstance = GetModuleHandle(NULL);
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "OpenGL";

	if (!RegisterClass(&wc)) {
	    MessageBox(NULL, "RegisterClass() failed:  "
		       "Cannot register window class.", "Error", MB_OK);
	    return NULL;
	}
    }

    hWnd = CreateWindow("OpenGL", title, 
        WS_OVERLAPPED | WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP | WS_DISABLED | WS_EX_TOOLWINDOW,
			x, y, width, height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
	MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
		   "Error", MB_OK);
	return NULL;
    }

    // Transparent
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);

    LONG cur_style = GetWindowLong(hWnd, GWL_EXSTYLE);
    SetWindowLong(hWnd, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);

    // Top most
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    hDC = GetDC(hWnd);

    /* there is no guarantee that the contents of the stack that become
       the pfd are zeroed, therefore _make sure_ to clear these bits. */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
    pfd.iPixelType   = type;
    pfd.cColorBits   = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
	MessageBox(NULL, "ChoosePixelFormat() failed:  "
		   "Cannot find a suitable pixel format.", "Error", MB_OK); 
	return 0;
    } 
 
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
	MessageBox(NULL, "SetPixelFormat() failed:  "
		   "Cannot set format specified.", "Error", MB_OK);
	return 0;
    } 

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    ReleaseDC(hWnd, hDC);

    return hWnd;
}    

char animations[6][3] = {"SD", "LA", "LD", "Wg", "R1", "R2"};
char directions[8][3] = {"S ", "SW", "W ", "NW", "N ", "NE", "E ", "SE"};

unsigned int catTextures[6][8][8];
unsigned int catAnimationFrames[6][8];

void loadGlAssets()
{

    for (int anim = 0; anim < 6; anim++)
    {
        for (int dir = 0; dir < 8; dir++)
        {
            char filestr[] = "./imgs/cat/00-00.0.png";
            // Anim Name
            filestr[11] = animations[anim][0];
            filestr[12] = animations[anim][1];

            // Direction name
            filestr[14] = directions[dir][0];
            filestr[15] = directions[dir][1];

            // printf(filestr);

            int frame;
            for(int frame = 0; frame < 8; frame++)
            {
                // Frame
                char snum[2];
                itoa(frame, snum, 10);
                filestr[17] = snum[0];

                if (!fileExists(filestr))
                {
                    frame++;
                    break;
                }

                // Loading Texture
                unsigned int texture;   
                glGenTextures(1, &texture); 
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                int width, height, nrChannels;
                unsigned char *data = stbi_load(filestr, &width, &height, &nrChannels, 0);   

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);

                catTextures[anim][dir][frame] = texture;
            }

            catAnimationFrames[anim][dir] = frame-1;
        }
    }
}

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{
    HDC hDC;				/* device context */
    HGLRC hRC;				/* opengl context */
    HWND  hWnd;				/* window */
    MSG   msg;				/* message */

    HMONITOR hmon = MonitorFromWindow(NULL,
                                    MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hmon, &mi);

    int mWidth  = mi.rcMonitor.right - mi.rcMonitor.left;
    int mHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

    char* windowName = "Cat";

    hWnd = CreateOpenGLWindow(windowName, 0, 0, mWidth, mHeight, PFD_TYPE_RGBA, 0);
    if (hWnd == NULL)
	exit(1);

    hDC = GetDC(hWnd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    ShowWindow(hWnd, nCmdShow);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // printf("%d", fileExists("./imgs/cats/LA-E .0.png"));

    loadGlAssets();

    while(GetMessage(&msg, hWnd, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    wglMakeCurrent(NULL, NULL);
    ReleaseDC(hWnd, hDC);
    wglDeleteContext(hRC);
    DestroyWindow(hWnd);

    return msg.wParam;
}