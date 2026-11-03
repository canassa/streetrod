/* F_0A544 @ 0000:A544 (linear 0x0A544, 1607 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_098BA(int);

static void near F_0A544(void)
{
    struct Bitmap far *pic4;
    int h8;
    int r;
    int flag8;
    struct Bitmap far *pic15;
    struct Bitmap far *pic24;
    struct Blt blt;
    struct Cell *p3;
    int v30;
    struct Bitmap far *pic58;
    int v2a;
    int i2;
    int v32;
    int minx;
    int miny;
    int v38l;
    struct Bitmap far *pic3;
    struct Cell *p2m;
    int w8;
    int cntz;
    struct Blt blt2k;
    struct Bitmap far *pic6m;
    int savedl;

    flag8 = 0;
    F_120FE(-4);
    F_113A2(0xbe);
    F_119B8(9, 8, 0x14, 2);
    g_8240 = 0;
    savedl = g_04c2;
    g_04c2 = 0xc8;
    (*g_78b6)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_118D4(0x408, -1);
    g_04c2 = savedl;
    (*g_present)();
    F_03A83(0x2ee);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    g_0284 = 1;
    pic15 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic15);
    F_15C95(g_4c28);
    g_4f4c = 1;
    r = F_15C4A(g_5318, 0);
    r = F_15C4A(g_5328, 0);
    (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
    g_8240 = 0;
    (*g_present)();
    g_8176 = 0x4c1;
    F_098BA(0x15);
    F_181C8(4, g_7648, g_7d84->fe, g_7d84->ff, 1, 0x98, 0, g_5030[g_827e]);
    g_8176 = 0x4dc;
    pic24 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic24);
    pic3 = F_15BBE(6, 1);
    pic4 = F_1934E(0x50, 0x2c, 0, 3);
    p3 = g_526a;
    blt.w = g_526a[0].w;
    blt.h = g_526a[0].h;
    blt.sx = g_526a[0].sx;
    blt.sy = g_526a[0].sy;
    blt.dx = g_526a[0].x - 0x50;
    blt.dy = 0x2c - g_526a[0].h;
    (*g_78aa)(pic3, pic15, &blt, 1);
    { }
    blt.w = p3->w;
    blt.h = p3->h;
    blt.sx = p3->x - 0x50;
    blt.sy = 0x2c - p3->h;
    blt.dx = blt.sx;
    blt.dy = blt.sy;
    (*g_78a2)(pic15, pic24, &blt, 1);
    F_03A83(10);
    for (i2 = 1; i2 < 0x14; i2++) {
        if (i2 == 13)
            F_10AFD(0x14);
        p3 = &g_526a[i2];
        F_19ED0(pic24, pic4, 1);
        blt.w = p3->w;
        blt.h = p3->h;
        blt.sx = p3->sx;
        blt.sy = p3->sy;
        blt.dx = p3->x - 0x50;
        blt.dy = 0x2c - p3->h;
        (*g_78aa)(pic3, pic4, &blt, 1);
        blt.w = 0x50;
        blt.h = 0x2c;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0x78;
        blt.dy = 0x45;
        if (i2 < 0x13) {
            F_10AFD(10);
            (*g_78a2)(pic4, g_vid.front, &blt, 1);
        }
        if (i2 == 0x12)
            (*g_78a2)(pic4, g_vid.back, &blt, 1);
    }
    F_193D4();
    if (flag8 == 0) {
        pic58 = F_15BBE(7, 1);
        cntz = 9;
        if (g_vid.mode == -6) {
            F_1929A(10000);
            cntz--;
        }
        v32 = v38l = 0x28;
        v2a = v30 = -0x28;
        for (i2 = 0; i2 < cntz; i2++) {
            p2m = &g_52e2[i2];
            w8 = max((int)p2m->w + (int)p2m->x, v32 + v2a) - (minx = min((int)p2m->x, v32));
            h8 = max((int)p2m->h + (int)p2m->y, v38l + v2a);
            h8 -= miny = min((int)p2m->y, v38l);
            pic6m = F_1934E(w8, h8, 0, 2);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = minx;
            blt2k.sy = miny;
            blt2k.dx = 0;
            blt2k.dy = 0;
            (*g_78a2)(g_vid.back, pic6m, &blt2k, 1);
            blt2k.w = v2a = p2m->w;
            blt2k.h = v30 = p2m->h;
            blt2k.sx = p2m->sx;
            blt2k.sy = p2m->sy;
            blt2k.dx = (v32 = p2m->x) - minx;
            blt2k.dy = (v38l = p2m->y) - miny;
            (*g_78aa)(pic58, pic6m, &blt2k, 1);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = 0;
            blt2k.sy = 0;
            blt2k.dx = minx;
            blt2k.dy = miny;
            F_10AFD(8);
            (*g_78a2)(pic6m, g_vid.front, &blt2k, 1);
            F_19438(1);
        }
    }
    blt2k.sx = minx;
    blt2k.sy = miny;
    F_03A83(0x50);
    F_10AFD(1);
    (*g_78a2)(g_vid.back, g_vid.front, &blt2k, 1);
    F_10AFD(1);
    (*g_78a2)(pic4, g_vid.front, &blt, 1);
    (*g_78a2)(pic4, g_vid.back, &blt, 1);
    F_03A83(0x32);
    F_03AB8(0x183c);
    g_8240 = 0;
    F_1923C();
    F_15C95(g_5328);
    F_15C95(g_5318);
}
