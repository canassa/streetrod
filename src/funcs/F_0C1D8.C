/* F_0C1D8 @ 0000:C1D8 (linear 0x0C1D8, 559 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0C1D8(void)
{
    struct Bitmap far *a, far *b;
    struct Blt pkt;
    int fast, i;

    fast = g_vid.mode == -2 && g_0254 == 0;
    F_16BE3();
    F_193D4();
    a = F_15BBE(0x4eb, 1);
    b = F_15BBE(0x4ec, 1);
    pkt.w = a->width;
    pkt.h = 0x5a;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = 0xb;
    (*g_78aa)(a, g_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height - 0x59;
    pkt.sy = 0x59;
    pkt.sx = pkt.dx = 0;
    pkt.dy = (fast ? 0x5a : 0x59) + 0xb;
    (*g_78aa)(a, g_vid.back, &pkt, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = fast ? 0x78 : 0x77;
    (*g_78a2)(b, g_vid.back, &pkt, 1);
    F_10414(g_82c2);
    (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
    if (g_78e0.f0 > 0x5a) {
        for (i = 0; i < 3; i++) {
            a = F_15BBE(g_53d4[i].id, 1);
            pkt.w = a->width;
            pkt.h = a->height;
            pkt.sx = pkt.sy = 0;
            pkt.dx = g_53d4[i].x;
            pkt.dy = g_53d4[i].y;
            F_10AFD(0x14 - 8 * i);
            (*g_78aa)(a, g_vid.back, &pkt, 1);
            (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
        }
    }
    F_120FE(-2);
    (*g_present)();
    F_00E24();
    F_03A83(0x64);
    F_00E24();
    F_193D4();
}
