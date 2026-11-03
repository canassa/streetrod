/* F_1309C: draw the current pair of slot pictures: the strip from g_vid.f8 and the sprite g_82ac,
   copying the area to the front buffer when the screen is shadowed. */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1287B(void);

void F_1309C(void)
{
    int next;
    int ys;
    int pair;
    int y2;
    int xs;
    int x2;
    struct Blt blt;

    pair = F_1287B() * 2;
    next = (g_82ac == 0x446) ? pair++ : pair + 1;
    F_120FE(-3);
    blt.w = 0x20;
    xs = blt.dx = (blt.sx = g_071a[next * 2]) + 0x60;
    ys = blt.dy = (blt.sy = g_071a[next * 2 + 1]) + 9;
    blt.h = g_vid.f8->height - blt.sy - 4;
    (*g_78a2)(g_vid.f8, g_vid.back, &blt, 1);
    F_1A734(g_vid.back, g_82ac, (unsigned char)(x2 = g_071a[pair * 2] + 0x60), (unsigned char)(y2 = g_071a[pair * 2 + 1] + 9), 0);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x2 > xs ? xs : x2, y2 > ys ? ys : y2, blt.w + x2 + 0x18, blt.h + y2 + 0x18);
}
