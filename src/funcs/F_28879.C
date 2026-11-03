/* F_28879 @ 2645:2429 (linear 0x28879, 1369 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_28879(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *(far *load)(struct Bitmap far *, unsigned char, unsigned char);
    struct Blt pkt1;
    int sel;
    struct Bitmap far *bm24;
    int il;
    int idx;

    F_193D4();
    bm1 = F_15BBE(0x3ed, 2);
    pkt1.w = bm1->width;
    pkt1.h = bm1->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_78a2)(bm1, g_8188, &pkt1, 1);
    F_19438(1);
    sel = *(int *)&g_8aca == 0 ? g_0284 : 0;
    bm24 = F_15BBE(g_5e62[sel], 2);
    pkt1.w = bm24->width;
    pkt1.h = bm24->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_78a2)(bm24, g_82b4, &pkt1, 1);
    F_19438(1);
    if (*(int *)&g_8aca == 0) {
        bm24 = F_15BBE(0x4ed, 2);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (g_vid.mode == -2 ? 0x63 : 0) + 1;
        (*g_78a2)(bm24, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt1, 1);
        F_19438(1);
        bm24 = F_15BBE(0x4ee, 2);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (g_vid.mode == -2 ? 0x63 : 0) + 0x2c;
        (*g_78a2)(bm24, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt1, 1);
        F_19438(1);
    }
    g_767e = F_15BBE(0x4f3, 1);
    if (*(int *)&g_8aca == 0) {
        g_764a = F_15BBE(0x3f0, 1);
        pkt1.w = g_764a->width;
        pkt1.h = g_764a->height;
        pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x2c;
        pkt1.dy = 0xc;
        (*g_78aa)(g_764a, g_767e, &pkt1, 1);
        F_1941E(1);
    }
    pkt1.w = g_767e->width;
    pkt1.h = g_767e->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0xc8 - g_767e->height;
    (*g_78a2)(g_767e, g_8188, &pkt1, 1);
    F_1941E(1);
    g_72f4 = F_15BBE(0x3ee, 1);
    g_8bca = F_15BBE(0x3ef, 1);
    g_8184 = F_15BBE(0x3f1, 1);
    g_8b7e = F_15BBE(0x4db, 1);
    if (*(int *)&g_8aca) {
        idx = g_8df0[g_7648].kind;
        g_6e60 = F_15BBE(0x3f5, 1);
        g_7aae = g_6e60->height;
        g_8ace[0] = F_15BBE(g_5e14[idx][0], 1);
        g_8ace[1] = F_15BBE(g_5e14[idx][1], 1);
        g_8ace[2] = F_15BBE(g_5e14[idx][2], 2);
        load = g_vid.mode == -2 ? F_2B857 : g_vid.mode == -6 ? F_2BCD5 : F_2BAC0;
        for (il = 0; il < 3; il++)
            g_8ace[3 + il] = (*load)(g_8ace[2], g_5e56[0][il], g_5e56[0][il]);
        for (il = 0; il < 3; il++)
            g_8ace[6 + il] = (*load)(g_8ace[5], g_5e56[1][il], g_5e56[1][il]);
        g_8ace[9] = F_15BBE(g_5e14[idx][3], 1);
        for (il = 0; il < 3; il++)
            g_8ace[10 + il] = (*load)(g_8ace[9], g_5e56[2][il], g_5e56[2][il]);
        g_7526[0] = g_8ace[0];
        g_7526[1] = F_15BBE(0x4d9, 1);
        g_7526[2] = F_15BBE(0x4da, 2);
        g_7526[2] = g_7526[3] = (*load)(g_7526[2], g_5e56[0][0], g_5e56[0][0]);
        F_19438(1);
        for (il = 0; il < 3; il++)
            g_7526[4 + il] = g_7526[3];
        g_7526[9] = F_15BBE(0x4de, 1);
        for (il = 0; il < 3; il++)
            g_7526[10 + il] = (*load)(g_7526[9], g_5e56[2][il], g_5e56[2][il]);
    }
}
