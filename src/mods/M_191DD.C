#include "STRUCTS.H"
#include "M_18BDC.H"
#include "GBSS.H"
#include "M_00000.H"
#include "M_0F38A.H"
#include "M_1923C.H"

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
    (*b_78a2)(r_vid.back, g_6ccc, &pkt, 1);
}
