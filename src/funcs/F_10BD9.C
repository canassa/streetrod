/* F_10BD9 @ 0F38:1859 (linear 0x10BD9, 403 bytes) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

void F_10BD9(struct Bitmap far *front, struct Bitmap far *back)
{
    int i;

    g_vid.front = front;
    g_vid.back = back;
    g_vid.f8 = 0;
    g_vid.f16 = 1;
    switch (g_vid.mode = front->flags) {
    case -2:
        g_vid.planes = 4;
        g_vid.shift = 3;
        g_vid.bpp = 1;
        g_vid.mask = -8;
        g_style = &g_03a0;
        outp(0x3d4, 0xc);
        outp(0x3d5, 0);
        outp(0x3d4, 0xd);
        outp(0x3d5, 0);
        break;
    default:
        g_vid.planes = 1;
        g_vid.shift = 2;
        g_vid.bpp = 2;
        g_vid.mask = -4;
        g_style = &g_03f0;
        g_vid.front->flags = -3;
        break;
    case -6:
        g_vid.planes = 1;
        g_vid.shift = 1;
        g_vid.bpp = 4;
        g_vid.mask = -2;
        g_style = &g_03a0;
        break;
    }
    g_colors = g_style->pal[1];
    g_8266 = g_8268 = -1;
    i = 5;
    do {
        i--;
        g_7568[i].id = g_7568[i].f2 = -1;
    } while (i);
    (*g_present)();
    F_13F7F();
    g_8247 = 1;
    g_7316[0] = g_vid.front;
    g_7316[1] = g_vid.back;
    g_7316[2] = &g_screens[2];
    if (g_vid.mode == -2) {
        *g_7316[2] = *g_vid.back;
        g_7316[2]->bits.w.off = 0x4000;
        g_7316[2]->bits.w.seg = 0xa000;
        g_7316[3] = &g_2e68_0000;
        *g_7316[3] = *g_vid.back;
        g_7316[3]->bits.w.off = 0x6000;
        g_7316[3]->bits.w.seg = 0xa000;
    }
}
