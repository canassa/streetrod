/* F_118D4: blit bitmap number a to the back buffer at (g_04be, g_04c0), then copy the rectangle to the front
   buffer. b >= 0 first fills the screen with color b. */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_118D4(int a, int b)
{
    struct Bitmap far *p;
    long unused;
    struct Blt pkt;

    F_120FE(-3);
    p = F_15BBE(a, 2);
    if (b >= 0)
        F_1169A(0, 0, 320, g_04c2, b);
    pkt.w = p->width;
    pkt.h = p->height > g_04c2 ? g_04c2 : p->height;
    pkt.sx = pkt.sy = pkt.fc = 0;
    pkt.dx = g_04be;
    pkt.dy = g_04c0;
    (*g_78a2)(p, g_vid.back, &pkt, 1);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, g_04be, g_04c0, pkt.dx + pkt.w - 1, pkt.dy + pkt.h - 1);
    g_04be = g_04c0 = 0;
    F_19438(1);
}
