/* F_10677 @ 0F38:12F7 (linear 0x10677, 507 bytes)

   Draws the car sprite at the top of the screen and, for each of the g_6c34->n points of the
   track, the wheel sprite (the position comes from the (x, y) pair, the source rectangle from
   g_038c). F_10872 is the per-frame update that draws the same pieces. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_10677(int a, int b)
{
    struct Blt pk5;
    struct Bitmap far *cm2;
    struct Blt p2;
    struct Blt p3;
    int x;
    struct Bitmap far *bm19;
    int i;
    struct Bitmap far *bm28;
    int y;

    g_6c34 = &g_036e[a ? 0 : b == 3 ? 1 : 2];
    bm19 = F_15BBE(g_6c34->id1, 2);
    pk5.w = 0x30;
    pk5.h = 0x2b;
    pk5.sx = pk5.sy = 0;
    pk5.dx = 0x108;
    pk5.dy = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x17;
    (*g_78a2)(bm19, g_58e4 ? g_755c : g_7678, &pk5, 1);
    bm28 = F_15BBE(g_6c34->id2, 2);
    p3.w = p2.w = (pk5.w = bm28->width) + 8;
    p3.h = p2.h = pk5.h = bm28->height;
    p3.sx = p3.sy = 0;
    p2.dx = p2.dy = 0;
    pk5.sx = pk5.sy = pk5.dy = 0;
    for (i = 0; i < g_6c34->n; i++) {
        x = g_6c34->pt[i * 2];
        y = g_6c34->pt[i * 2 + 1];
        p2.sx = x & 0xfff8;
        p2.sy = y;
        cm2 = F_1A89E(bm19, &p2, 1);
        p3.dx = g_038c[i].a;
        p3.dy = g_038c[i].b;
        (*g_78a2)(cm2, g_82b4, &p3, 1);
        pk5.dx = x - p2.sx;
        F_1A2D0(bm28, cm2, &pk5, 0);
        p3.dy += 0x13;
        (*g_78a2)(cm2, g_82b4, &p3, 1);
        F_1941E(1);
    }
    g_6c36 = -1;
    F_19438(2);
}
