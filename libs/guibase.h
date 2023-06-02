#include "vectormath.h"
#include "basefont.h"
#include <stdio.h>

typedef uint32_t u32;

const int intsize = sizeof(int);

struct {
    v2 size;
    v2 position;
    v2 anchorpoint;
    u32 color;
    int textsize;
    void *text;
} typedef GUI_BASE;

struct {
    void* memory;
    int width;
    int height;
} typedef render_buffer;

u32* move(render_buffer rb, u32* pixels, int x, int y)
{
    return pixels += y*rb.width + x;
}

void rendergui(render_buffer rb, GUI_BASE gb)
{
    // u32* pixels = (u32*) rb.memory;

    // //Acounting for position and anchorpoint
    // v2 sp = Subv2s(gb.position, Multiv2s(gb.anchorpoint, gb.size));
    // pixels += (int) sp.x * rb.width + (int) sp.y;

    // u32* tpixels = pixels;

    // int width = sp.x + gb.size.x;

    // for (int y = 0; y < gb.size.y; y++)
    // {
    //     for (int x = 0; x < gb.size.x; x++)
    //     {
    //         pixels++;
    //         *pixels = gb.color;
    //     }
    //     pixels += rb.width - (int) gb.size.x;
    // }

    // char *text = gb.text;

    // const int fontsize = 2;
    // const int charspacing = 10;

    //Text Render
    // tpixels = move(rb, tpixels - charspacing, 5 * gb.textsize * fontsize/-2, 2);

    // for (int y = 0; y < 5 * fontsize; y++)
    // {
    //     for (int x = 0; x < 5 * gb.textsize * fontsize; x++)
    //     {
    //         tpixels += 1 + (x % (5*fontsize) == 0) * charspacing;
    //         char c = *(text + x/(fontsize*5));
    //         char On = basefont[c][y/fontsize * 5 + (x/(fontsize)) % 5];
    //         *tpixels = *tpixels * (1-On) + 0xffffff*On;

    //     }
    //     tpixels += rb.width - 5 * gb.textsize * fontsize - gb.textsize*charspacing;
    // }
}