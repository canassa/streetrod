/* F_102D3 @ 0F38:0F53 (linear 0x102D3, 321 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_102D3(void)
{
    int i6;
    struct Bitmap far *bm;
    struct Blt pkt;

    if (g_vid.mode != -2) {
        pkt.w = 0xb8;
        pkt.h = 0x1a;
        pkt.sx = pkt.sy = 0;
        pkt.dx = 0x88;
        for (i6 = 0; i6 < 3; i6++) {
            bm = F_15BBE(g_58e8[i6], 1);
            if (g_vid.mode != -2 || i6 < 2) {
                g_6c28[i6] = g_82b4;
                g_6c22[i6] = g_0352[i6];
            } else {
                g_6c28[i6] = g_7678;
                g_6c22[i6] = g_0352[i6] + 0x66;
            }
            pkt.dy = g_6c22[i6];
            (*g_78a2)(bm, g_6c28[i6], &pkt, 1);
            F_1941E(1);
        }
    } else {
        for (i6 = 0; i6 < 3; i6++)
            g_6c28[i6] = F_15BBE(g_58e8[i6], 1);
    }
}
