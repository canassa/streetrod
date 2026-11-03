/* F_0F38A @ 0F38:000A (linear 0x0F38A, 142 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0F38A(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = 0x108;
    pkt.sy = (g_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    pkt.dx = x & 0xfff8;
    pkt.dy = y;
    (*g_78a2)(g_vid.mode == -2 ? g_7678 : g_82b4, g_7678, &pkt, 1);
}
