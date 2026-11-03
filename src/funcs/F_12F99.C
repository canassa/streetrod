/* F_12F99: draw item picture n at the current slot position, 20 above it, via the g_vid.f8 buffer,
   and copy the area to the front buffer when the screen is shadowed. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_12F99(int n)
{
    struct Bitmap far *pict;
    long unused;
    int w;
    struct Blt move;
    int h;
    int px;
    int py;

    F_12909(&px, &py);
    py -= 0x14;
    F_120FE(-3);
    pict = F_15BBE(n, 2);
    if (n != 0x434 && n != 0x435 && n != 0x436)
        py -= pict->height;
    move.sx = px;
    move.dx = px += 0x60;
    move.sy = py;
    move.dy = py += 9;
    w = move.w = pict->width + 8;
    h = move.h = move.fc = pict->height;
    (*g_78a2)(g_vid.f8, g_vid.back, &move, 1);
    F_19438(1);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, px, py, px + w, py + h);
}
