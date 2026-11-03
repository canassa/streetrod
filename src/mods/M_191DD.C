#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_191DD: allocate the 0x30 x 0x29 bitmap g_6ccc and clear a 200 x 68 rectangle of it */


void F_191DD(void)
{
    struct Blt pkt;

    g_6ccc = F_1934E(0x30, 0x29, 0, 0);
    pkt.w = g_6ccc->width;
    pkt.h = g_6ccc->height;
    pkt.sx = 200;
    pkt.sy = 68;
    pkt.dx = pkt.dy = 0;
    (*g_78a2)(g_vid.back, g_6ccc, &pkt, 1);
}
