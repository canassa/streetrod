/* F_0F418 @ 0E9F:0A28 (linear 0x0F418, 159 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0F418(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = x & 0xfff8;
    pkt.sy = y;
    pkt.dx = 0x108;
    pkt.dy = (g_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    (*g_78a2)(g_58e4 ? g_755c : g_7678, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt, 1);
}
