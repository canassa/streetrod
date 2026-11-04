#include "STRUCTS.H"
#include "M_0F38A.H"
#include "FONTDATA.H"
#include "M_00000.H"
#include "M_15396.H"
#include "M_174A6.H"
#include "M_193D4.H"
#include "M_19ED0.H"
#include <conio.h>

static struct Track *g_6c34;               /* DS:6C34 */
static int g_6c36;                         /* DS:6C36 */

/* F_10677 @ 0F38:12F7 (linear 0x10677, 507 bytes)

   Draws the car sprite at the top of the screen and, for each of the g_6c34->n points of the
   track, the wheel sprite (the position comes from the (x, y) pair, the source rectangle from
   g_038c). F_10872 is the per-frame update that draws the same pieces. */


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
    pk5.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x17;
    (*b_78a2)(bm19, g_58e4 ? g755cu : bm7678, &pk5, 1);
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
        (*b_78a2)(cm2, g82b4s, &p3, 1);
        pk5.dx = x - p2.sx;
        F_1A2D0(bm28, cm2, &pk5, 0);
        p3.dy += 0x13;
        (*b_78a2)(cm2, g82b4s, &p3, 1);
        F_1941E(1);
    }
    g_6c36 = -1;
    F_19438(2);
}

/* F_10872 @ 0F38:14F2 (linear 0x10872, 334 bytes) */


void F_10872(struct Bitmap far *dst, struct Eng *e)
{
    struct Blt16 pkt1;
    int x, idx3;

    if (e->f24 == 0)
        idx3 = e->f18;
    else if (e->f18 == 0)
        idx3 = 0;
    else
        idx3 = e->f24 - 1;
    pkt1.h = g_6c34->h;
    if (g_6c36 >= 0) {
        pkt1.sx = g_038c[g_6c36].a;
        pkt1.sy = g_038c[g_6c36].b;
        x = g_6c34->pt[g_6c36 * 2] & 0xfff8;
        pkt1.dx = x + 0x108;
        pkt1.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[g_6c36 * 2 + 1] + 0x17;
        pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
        (*b_78a2)(g82b4s, dst, (struct Blt *)&pkt1, 1);
    }
    pkt1.sx = g_038c[idx3].a;
    pkt1.sy = g_038c[idx3].b + 0x13;
    x = g_6c34->pt[idx3 * 2] & 0xfff8;
    pkt1.dx = x + 0x108;
    pkt1.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[idx3 * 2 + 1] + 0x17;
    pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
    (*b_78a2)(g82b4s, dst, (struct Blt *)&pkt1, 1);
    g_6c36 = idx3;
    e->f1a = 0;
}

/* F_109C0 @ 0F38:1640 (linear 0x109C0, 64 bytes) */


void F_109C0(char far *p, int c, unsigned n)
{
    char far *q;

    for (q = p; n; n--)
        *q++ = c;
}

/* F_10A00 @ 0F38:1680 (linear 0x10A00, 253 bytes)

   Clips the rectangle in to the destination bitmap, then to the
   source bitmap; the result goes to out. Returns 1 if anything is left. The x values of in
   are shifted down by r_vid.shift. */


int F_10A00(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out)
{
    int t1, t2;
    int w1, h;
    int sx, sy, dx, dy;

    sx = in->sx >> r_vid.shift;
    sy = in->sy;
    w1 = in->w >> r_vid.shift;
    h = in->h;
    if (sx + w1 > dst->pitch)
        w1 = dst->pitch - sx;
    if (sy + h > dst->height)
        h = dst->height - sy;
    dx = in->dx >> r_vid.shift;
    dy = in->dy;
    if (dy < 0) {
        h += dy;
        sy -= dy;
        dy = 0;
    }
    if ((t1 = src->height) < dy + h)
        h = t1 - dy;
    if (dx < 0) {
        w1 += dx;
        sx -= dx;
        dx = 0;
    }
    if ((t2 = src->pitch) < dx + w1)
        w1 = t2 - dx;
    out->sx = sx;
    out->sy = sy;
    out->dx = dx;
    out->dy = dy;
    out->w = w1;
    out->h = h;
    return out->w > 0 && out->h > 0;
}

/* F_10AFD @ 0F38:177D (linear 0x10AFD, 75 bytes) */

#pragma intrinsic(inp)

void F_10AFD(int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (r_vid.mode != -4) {
            while (inp(0x3da) & 8)
                ;
            while (!(inp(0x3da) & 8))
                ;
        } else {
            while (!(inp(0x3ba) & 0x80))
                ;
            while (inp(0x3ba) & 0x80)
                ;
        }
    }
}

/* F_10B48 @ 0F38:17C8 (linear 0x10B48, 52 bytes) */


int F_10B48(char *s)
{
    int w;

    for (w = 0; *s; s++)
        w += g_font[*s - 32].w;
    return w;
}
