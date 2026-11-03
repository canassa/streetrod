/* F_12A17: draw picture n (item id) at the current slot position and restore the background strip
   behind it. With the screen shadowed the picture is first copied into a bitmap 8 pixels wider. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_12A17(int n)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused;
    struct Blt move;
    int px;
    int height;
    int flag;
    int ypos;

    flag = 0;
    F_12964(&px, &ypos);
    F_120FE(-4);
    saved = pict = F_15BBE(n, 2);
    ypos -= saved->height;
    height = move.h = move.fc = saved->height;
    px -= 2;
    move.sy = ypos;
    move.dy = ypos += 9;
    move.dx = (move.sx = px) + 0x60;
    move.w = 6;
    (*g_78a6)(g_vid.f8, g_vid.back, &move);
    if (flag = px & ~g_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, flag, saved);
    }
    px = (px & g_vid.mask) + 0x60;
    (*g_78ce)(0xd, 0x93, px, (n == 0x43f && g_vid.f16) ? 0xd - height : ypos, ypos, saved);
    F_120FE(-2);
    F_19438(1);
    if (pict != saved)
        F_19438(1);
}
