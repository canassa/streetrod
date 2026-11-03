#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>

/* F_11E9C @ 0F38:2B1C (linear 0x11E9C, 12 bytes) */


unsigned F_11E9C(unsigned n)
{
    return n * g_cursor.size;
}

/* F_11EA8: set up the off-screen work bitmap and the scratch strip used by the pop-up code */


void F_11EA8(void)
{
    unsigned bytes;
    int size;
    struct Bitmap far *mem;
    struct Bitmap far *img;
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    unsigned far *fp;
    int step;

    bytes = 0x600;
    size = (g_vid.planes + 1) * bytes + 0x60;
    if (g_vid.mode != -2)
        size += (0x18 >> g_vid.shift) << 4;
    mem = (struct Bitmap far *)F_03709(size, 1);
    mem->bits.p = (char far *)mem + 0x30;
    img = F_0376B(0x18 << g_vid.shift, 0x40, 1, -1, mem);
    g_cursor.save = (struct Bitmap far *)(img->alt.p + bytes);
    F_03904(g_cursor.save, g_vid.mode == -2 ? (char far *)0xa0001f40L : (char far *)(g_cursor.save + 1), 0L,
            (0x18 >> g_vid.shift) << g_vid.shift, 0x10, g_vid.mode == -2 ? -2 : -1);
    memset(&g_cursor.clip, 0, 12);
    g_cursor.state = 0;
    pict = F_15BBE(2, 2);
    if (pict == 0)
        out_of_memory();
    blt.w = 0x18;
    blt.h = 0x40;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_78a2)(pict, img, &blt, 3);
    for (step = 1; step <= ~g_vid.mask; step++) {
        tmp = F_1934E(0x18, 0x40, 1, 1);
        F_1A03F(pict, step, tmp);
        blt.dx += 0x18;
        (*g_78a2)(tmp, img, &blt, 3);
        F_1941E(1);
    }
    F_193D4();
    g_cursor.shape = img->bits.p;
    g_cursor.mask = img->alt.p;
    if (g_vid.mode == -2) {
        fp = (unsigned far *)g_cursor.mask;
        step = img->planesize >> 3;
        while (step--) {
            *fp = ~*fp;
            fp++;
        }
    }
    g_cursor.size = img->pitch << 4;
    g_cursor.n = 1;
    g_cursor.off = F_11E9C(1);
    g_cursor.r.x2 = -1;
    g_472a = 0x9e;
    g_472c = 0x80;
    g_4746 = 0;
    g_4748 = 0;
    g_8247 = 2;
}

/* F_120FE: mouse cursor control. mode >= 0 selects cursor shape number mode and moves it to the
   (clamped) mouse position; -1/-2 show it and -3/-4 hide it (nested by a counter). */


void F_120FE(int mode)
{
    int a;
    int b;

    a = g_472a;
    b = g_472c;
    a = a < 0 ? 0 : a > 319 ? 319 : a;
    b = b < 0 ? 0 : b > 199 ? 199 : b;
    if (mode >= 0) {
        g_4742 = g_06ec[g_cursor.n = mode].a;
        g_4744 = g_06ec[mode].b;
        g_cursor.off = F_11E9C(g_cursor.n);
        F_1220B(a, b);
    } else if (mode >= -2) {
        if (++g_4733 > 0) {
            g_cursor.state = (mode == -1) + 1;
            if (g_cursor.r.x2 >= 0)
                F_1220B(g_cursor.r.x1, g_cursor.r.y1);
            else
                F_1220B(a, b);
            g_cursor.state = 1;
        }
    } else {
        if (--g_4733 >= 0) {
            g_cursor.state = -((mode == -3) - 0xff);
            if (g_cursor.r.x2 >= 0)
                F_1220B(g_cursor.r.x1, g_cursor.r.y1);
            else
                F_1220B(a, b);
            g_cursor.state = 0;
        }
    }
}

/* F_1220B: move the saved-background cursor strip to (x, y): restore what it covered, save and draw anew */


void F_1220B(int x, int y)
{
    int a;
    int b;

    if (g_cursor.state) {
        if (g_cursor.clip.w > 0) {
            if (g_vid.mode == -2)
                F_24EFB(g_cursor.save->bits.p, g_cursor.clip.w >> 3, g_cursor.clip.h, g_vid.back->bits.p + 40 * g_cursor.clip.dy + (g_cursor.clip.dx >> 3), 0x18 >> g_vid.shift);
            else
                (*g_78a2)(g_cursor.save, g_vid.back, (struct Blt *)&g_cursor.clip, 1);
            g_cursor.clip.w = 0;
        } else {
            g_cursor.r.x1 = g_cursor.r.y1 = 1000;
            g_cursor.r.x2 = g_cursor.r.y2 = -1;
        }
        if (g_cursor.state < 0) {
            if (g_cursor.r.x2 < 0)
                return;
            if (g_cursor.state < -1)
                return;
            g_cursor.state = 1;
        } else {
            g_cursor.clip.dx = x & g_vid.mask;
            g_cursor.clip.w = 320 - g_cursor.clip.dx > 0x18 ? 0x18 : 320 - g_cursor.clip.dx;
            g_cursor.clip.h = 200 - y > 16 ? 16 : 200 - y;
            g_cursor.clip.dy = y;
            a = (x & ~g_vid.mask) * (0x18 >> g_vid.shift) + g_cursor.off;
            (*g_78b2)(g_cursor.shape + a, g_cursor.mask + a, g_cursor.save->bits.p, g_vid.back->bits.p, g_cursor.clip.dx >> g_vid.shift, y, g_cursor.clip.w >> g_vid.shift, g_cursor.clip.h);
        }
        a = x + 15;
        b = y + 15;
        if (g_vid.f16 && g_cursor.state == 1)
            F_1A92A(g_vid.back, g_vid.front, g_cursor.r.x1 > x ? x : g_cursor.r.x1, g_cursor.r.y1 > y ? y : g_cursor.r.y1, g_cursor.r.x2 < a ? a : g_cursor.r.x2, g_cursor.r.y2 < b ? b : g_cursor.r.y2);
        g_cursor.r.x1 = x;
        g_cursor.r.y1 = y;
        g_cursor.r.x2 = a;
        g_cursor.r.y2 = b;
    }
}
