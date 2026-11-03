/* F_2A32B @ 2645:3EDB (linear 0x2A32B, 178 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2874C(struct Blt *b);

static void near F_2A32B(void)
{
    struct Blt b;

    b.w = g_8184->width;
    b.h = g_8184->height;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = 0x12;
    (*g_78a2)(g_8184, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    b.w = g_8b7e->width;
    b.h = 2;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = g_8646 - 2;
    F_2874C(&b);
    (*g_78a2)(g_8b7e, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}
