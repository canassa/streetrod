#include <string.h>
#include "STRUCTS.H"
#include "M_11E9C.H"
#include "M_00000.H"
#include "M_02B9A.H"
#include "M_036A8.H"
#include "M_0F38A.H"
#include "M_15396.H"
#include "M_1923C.H"
#include "M_193D4.H"
#include "M_19ED0.H"
#include "M_24E78.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
void (far *ap78b2z)(char far *, char far *, char far *, char far *, int, int, int, int);
struct Cursor g_cursor2;

struct Pair g_06ec[4] = {{1, 1}, {2, 2}, {1, 1}, {8, 8}};   /* DS:06EC */

/* F_11E9C @ 0F38:2B1C (linear 0x11E9C, 12 bytes) */


unsigned F_11E9C(unsigned n)
{
    return n * g_cursor2.size;
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
    size = (r_vid.planes + 1) * bytes + 0x60;
    if (r_vid.mode != -2)
        size += (0x18 >> r_vid.shift) << 4;
    mem = (struct Bitmap far *)F_03709(size, 1);
    mem->bits.p = (char far *)mem + 0x30;
    img = F_0376B(0x18 << r_vid.shift, 0x40, 1, -1, mem);
    g_cursor2.save = (struct Bitmap far *)(img->alt.p + bytes);
    F_03904(g_cursor2.save, r_vid.mode == -2 ? (char far *)0xa0001f40L : (char far *)(g_cursor2.save + 1), 0L,
            (0x18 >> r_vid.shift) << r_vid.shift, 0x10, r_vid.mode == -2 ? -2 : -1);
    memset(&g_cursor2.clip, 0, 12);
    g_cursor2.state = 0;
    pict = F_15BBE(2, 2);
    if (pict == 0)
        out_of_memory();
    blt.w = 0x18;
    blt.h = 0x40;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*b_78a2)(pict, img, &blt, 3);
    for (step = 1; step <= ~r_vid.mask; step++) {
        tmp = F_1934E(0x18, 0x40, 1, 1);
        F_1A03F(pict, step, tmp);
        blt.dx += 0x18;
        (*b_78a2)(tmp, img, &blt, 3);
        F_1941E(1);
    }
    F_193D4();
    g_cursor2.shape = img->bits.p;
    g_cursor2.mask = img->alt.p;
    if (r_vid.mode == -2) {
        fp = (unsigned far *)g_cursor2.mask;
        step = img->planesize >> 3;
        while (step--) {
            *fp = ~*fp;
            fp++;
        }
    }
    g_cursor2.size = img->pitch << 4;
    g_cursor2.n = 1;
    g_cursor2.off = F_11E9C(1);
    g_cursor2.r.x2 = -1;
    g_472a = 0x9e;
    g_472c = 0x80;
    g_4746 = 0;
    g_4748 = 0;
    ap8247u = 2;
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
        g_4742 = g_06ec[g_cursor2.n = mode].a;
        g_4744 = g_06ec[mode].b;
        g_cursor2.off = F_11E9C(g_cursor2.n);
        F_1220B(a, b);
    } else if (mode >= -2) {
        if (++g_4733 > 0) {
            g_cursor2.state = (mode == -1) + 1;
            if (g_cursor2.r.x2 >= 0)
                F_1220B(g_cursor2.r.x1, g_cursor2.r.y1);
            else
                F_1220B(a, b);
            g_cursor2.state = 1;
        }
    } else {
        if (--g_4733 >= 0) {
            g_cursor2.state = -((mode == -3) - 0xff);
            if (g_cursor2.r.x2 >= 0)
                F_1220B(g_cursor2.r.x1, g_cursor2.r.y1);
            else
                F_1220B(a, b);
            g_cursor2.state = 0;
        }
    }
}

/* F_1220B: move the saved-background cursor strip to (x, y): restore what it covered, save and draw anew */


void F_1220B(int x, int y)
{
    int a;
    int b;

    if (g_cursor2.state) {
        if (g_cursor2.clip.w > 0) {
            if (r_vid.mode == -2)
                F_24EFB(g_cursor2.save->bits.p, g_cursor2.clip.w >> 3, g_cursor2.clip.h, r_vid.back->bits.p + 40 * g_cursor2.clip.dy + (g_cursor2.clip.dx >> 3), 0x18 >> r_vid.shift);
            else
                (*b_78a2)(g_cursor2.save, r_vid.back, (struct Blt *)&g_cursor2.clip, 1);
            g_cursor2.clip.w = 0;
        } else {
            g_cursor2.r.x1 = g_cursor2.r.y1 = 1000;
            g_cursor2.r.x2 = g_cursor2.r.y2 = -1;
        }
        if (g_cursor2.state < 0) {
            if (g_cursor2.r.x2 < 0)
                return;
            if (g_cursor2.state < -1)
                return;
            g_cursor2.state = 1;
        } else {
            g_cursor2.clip.dx = x & r_vid.mask;
            g_cursor2.clip.w = 320 - g_cursor2.clip.dx > 0x18 ? 0x18 : 320 - g_cursor2.clip.dx;
            g_cursor2.clip.h = 200 - y > 16 ? 16 : 200 - y;
            g_cursor2.clip.dy = y;
            a = (x & ~r_vid.mask) * (0x18 >> r_vid.shift) + g_cursor2.off;
            (*ap78b2z)(g_cursor2.shape + a, g_cursor2.mask + a, g_cursor2.save->bits.p, r_vid.back->bits.p, g_cursor2.clip.dx >> r_vid.shift, y, g_cursor2.clip.w >> r_vid.shift, g_cursor2.clip.h);
        }
        a = x + 15;
        b = y + 15;
        if (r_vid.f16 && g_cursor2.state == 1)
            F_1A92A(r_vid.back, r_vid.front, g_cursor2.r.x1 > x ? x : g_cursor2.r.x1, g_cursor2.r.y1 > y ? y : g_cursor2.r.y1, g_cursor2.r.x2 < a ? a : g_cursor2.r.x2, g_cursor2.r.y2 < b ? b : g_cursor2.r.y2);
        g_cursor2.r.x1 = x;
        g_cursor2.r.y1 = y;
        g_cursor2.r.x2 = a;
        g_cursor2.r.y2 = b;
    }
}
