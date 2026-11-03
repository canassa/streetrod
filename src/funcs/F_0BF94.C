/* F_0BF94 @ 0000:BF94 (linear 0x0BF94, 580 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <string.h>

static void near F_0BF94(void)
{
    struct Bitmap far *a, far *b;
    int endk, desc3;
    char *p2;
    int wk;
    char d, e, cl;
    int maxw2;
    struct Blt pkt;

    F_193D4();
    a = F_15BBE(0x456, 1);
    b = F_15BBE(0x4ef, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.dx = pkt.sx = pkt.sy = 0;
    pkt.dy = 0x45;
    (*g_78aa)(b, g_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = b->width;
    pkt.dy = 0x14;
    (*g_78a2)(a, g_vid.back, &pkt, 1);
    maxw2 = a->width - 8;
    g_vid.f16 = 0;
    g_colors = g_style->hilite;
    for (p2 = g_newName, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    (*g_78ae)(g_vid.back, 0x5e, 0x48, g_newName);
    *p2 = d;
    desc3 = g_7d86[g_7eb0->f2].f6;
    endk = desc3 + strcspn(g_2c01 + desc3 + 8, " `.") + 7;
    for (p2 = g_2c01 + desc3 + 1, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    e = g_2c01[endk + 1];
    g_2c01[endk + 1] = 0;
    (*g_78ae)(g_vid.back, 0x5e, 0x63, g_2c01 + desc3 + 1);
    g_2c01[endk + 1] = e;
    *p2 = d;
    (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
    g_vid.f16 = 1;
    F_120FE(-2);
    F_00E24();
}
