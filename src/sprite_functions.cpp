#include <windows.h>

HBITMAP createCompatibleBitmap(HDC hdc, int width, int height)
{
    // Get the number of color planes
    int nPlanes = GetDeviceCaps(hdc, PLANES);

    // Get the number of bits per pixel
    int nBitCount = GetDeviceCaps(hdc, BITSPIXEL);

    const void* lpBits = malloc((((width * nPlanes * nBitCount + 15) >> 4) << 1) * height);
    return CreateBitmap(width, height, nPlanes, nBitCount, lpBits);
}

void coverSpriteDisplacement(HDC hdc, HBRUSH brush, int x, int y, int nx, int ny, int width, int height)
{
    int xDif = nx-x;
    int yDif = ny-y;

    RECT rect;
    if (abs(xDif) > width || abs(yDif) > height)
    {
        // We can just fully cover with another square
        rect = {nx, ny, nx+width, ny+height};
        FillRect(hdc, &rect, brush);
    } else {
        // a a a
        // a b b b
        // a b b b
        //   b b b

        // c c c
        // d b b b
        // d b b b
        //   b b b

        int yMin = y;
        int yMax = y+height;

        // Covering Vertical Displacement
        if (yDif > 0)
        {
            rect = {x, y, x+width, ny};
            FillRect(hdc, &rect, brush);
            yMin = ny;
        } else {
            rect = {x, ny+height, x+width, y+height};
            FillRect(hdc, &rect, brush);
            yMax = ny+height;
        }

        if (xDif > 0)
        {
            rect = {x, yMin, nx, yMax};
            FillRect(hdc, &rect, brush);
        } else {
            rect = {nx+width, yMin, x+width, yMax};
            FillRect(hdc, &rect, brush);
        }
    }
}