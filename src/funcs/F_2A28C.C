/* F_2A28C @ 2645:3E3C (linear 0x2A28C, 159 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2A28C(void)
{
    struct Blt b;

    b.w = g_72f4->width;
    b.h = g_72f4->height;
    b.dx = b.sx = b.sy = 0;
    b.dy = 0x12;
    (*g_78aa)(g_72f4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    b.w = g_8bca->width;
    b.h = g_8bca->height;
    b.sx = b.sy = 0;
    b.dx = 0x58;
    b.dy = 0x5f;
    (*g_78aa)(g_8bca, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}
