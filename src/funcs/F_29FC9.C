/* F_29FC9 @ 2645:3B79 (linear 0x29FC9, 339 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_28781(struct Blt *b);
static void near F_2874C(struct Blt *b);

static void near F_29FC9(int x, int y, int n, int align, int clip, int mode)
{
    struct Blt b;

    n = 9 - n;
    if (n > 6)
        n = 6;
    if (n < 0)
        n = 0;
    b.w = g_5e9c[n].w;
    b.h = g_5e9c[n].h;
    b.sx = g_5e9c[n].sx;
    switch (mode) {
    case 1:
        b.sy = 0x66;
        break;
    case 2:
        b.sy = (g_vid.mode == -2 ? 0x63 : 0) + 1;
        break;
    case 3:
        b.sy = (g_vid.mode == -2 ? 0x63 : 0) + 0x2c;
        break;
    }
    if (align == 1)
        b.dx = x - b.w;
    else if (align == 0)
        b.dx = x - (b.w >> 1);
    else
        b.dx = x;
    if (align == 0)
        b.dy = y - (b.h >> 1);
    else
        b.dy = y - b.h;
    if (clip)
        F_28781(&b);
    else
        F_2874C(&b);
    if (mode == 1)
        (*g_78a2)(g_82b4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    else
        (*g_78a2)(g_vid.mode == -2 ? g_7678 : g_82b4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}
