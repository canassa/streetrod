#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_19ED0: copy a bitmap into another through the blit hook */


void F_19ED0(struct Bitmap far *src, struct Bitmap far *dst, int e)
{
    struct Blt pkt;

    F_0376B(src->width, src->height, dst->alt.p ? 1 : 0, -1, dst);
    pkt.w = src->width;
    pkt.h = src->height;
    pkt.dx = pkt.dy = pkt.sx = pkt.sy = 0;
    (*g_78a2)(src, dst, &pkt, e);
}

/* F_19F42: find the first and last set pixel column of a bitmap */


void F_19F42(struct Bitmap far *bm, int *left, int *right)
{
    int a, f, d;
    unsigned char c, e, b;
    char far *g, far *h;

    a = bm->pitch;
    f = bm->height;
    h = (g = bm->alt.p) + a - 1;
    d = 0;
    c = e = 0xff;
    for (; d < f; d++) {
        c &= *g;
        e &= *h;
        g += a;
        h += a;
    }
    for (d = 0, b = 0x80; d < 8; d++, b >>= 1)
        if (!(c & b))
            break;
    *left = d / g_vid.bpp;
    for (d = 0, b = 1; d < 8; d++, b <<= 1)
        if (!(e & b))
            break;
    *right = (bm->pitch << g_vid.shift) - 1;
    *right -= d / g_vid.bpp;
}

/* F_1A03F: shift a bitmap (pixels and mask) right by n pixels into a new, wider bitmap */


struct Bitmap far *F_1A03F(struct Bitmap far *src, int n, struct Bitmap far *dst)
{
    unsigned char acc1;
    int pitch;
    struct Bitmap far *out;
    unsigned char far *sp24;
    unsigned char acc2;
    int h3;
    unsigned char far *sp8;
    unsigned char far *dp25;
    int k;
    int compl;
    int i21;
    int extra8;
    unsigned char far *dp8;
    int shl5;
    int j2;
    int mask;

    shl5 = (n * g_vid.bpp) & 7;
    compl = 8 - shl5;
    mask = 0xff >> compl;
    h3 = src->height;
    pitch = src->pitch;
    if (n <= 0) {
        F_19ED0(src, dst, src->alt.p ? 3 : 1);
        return dst;
    }
    acc1 = 0xff;
    sp8 = (unsigned char far *)src->alt.p + pitch - 1;
    for (j2 = 0; j2 < h3; j2++) {
        acc1 &= *sp8;
        sp8 += pitch;
    }
    extra8 = (acc1 & mask) == mask ? 0 : 1 << g_vid.shift;
    out = F_0376B(src->width + extra8, src->height, src->alt.p ? 1 : 0, -1, dst);
    dp8 = (unsigned char far *)out->bits.p;
    sp8 = (unsigned char far *)src->bits.p;
    for (k = 0; k < h3 * g_vid.planes; k++) {
        for (i21 = 0, acc1 = 0; i21 < pitch; i21++) {
            *dp8++ = (*sp8 >> shl5) | (acc1 << compl);
            acc1 = mask & *sp8;
            sp8++;
        }
        if (extra8)
            *dp8++ = acc1 << compl;
    }
    if (src->alt.p) {
        dp25 = (unsigned char far *)out->alt.p;
        sp24 = (unsigned char far *)src->alt.p;
        for (k = 0; k < h3; k++) {
            for (i21 = 0, acc2 = 0xff; i21 < pitch; i21++) {
                *dp25++ = (*sp24 >> shl5) | (acc2 << compl);
                acc2 = mask & *sp24;
                sp24++;
            }
            if (extra8)
                *dp25++ = (acc2 << compl) | (0xff >> shl5);
        }
    }
    return out;
}

/* F_1A2D0: mask-blit src into dst, optionally and-ing the pixels together */


void F_1A2D0(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag)
{
    struct Blt a;
    struct Blt e;
    struct Bitmap far *j;
    char far *c;
    char far *g;
    int f, b, h, d, i, k, l, m;

    for (i = 0; i < 10; i++)
        ((int *)&e)[i] = ((int *)r)[i];
    f = r->dx & ~g_vid.mask;
    if (f > 0) {
        j = F_1934E(dst->width + 8, dst->height, dst->alt.p != 0L, 2);
        F_1A03F(dst, f, j);
        e.w = j->width;
    } else
        j = dst;
    (*g_78aa)(j, src, &e, 1);
    if (flag) {
        if (F_10A00(j, src, &e, &a)) {
            d = j->pitch;
            b = a.w;
            h = a.h;
            l = d - b;
            m = src->pitch - b;
            c = j->alt.p + a.sy * d + a.sx;
            g = src->alt.p + src->pitch * a.dy + a.dx;
            for (i = 0; i < h; i++) {
                for (k = 0; k < b; k++) {
                    *g &= *c;
                    g++;
                    c++;
                }
                c += l;
                g += m;
            }
        }
    }
    if (f)
        F_19438(1);
}

/* F_1A472: draw a masked bitmap into another at pixel (x, y), pre-shifting it to the byte phase */


void F_1A472(struct Bitmap far *src, struct Bitmap far *dst, int x, int y)
{
    unsigned char far *sm;
    int dpitch4;
    int sh5;
    unsigned char far *spn;
    unsigned char far *dm;
    int pstep5;
    int dskip6;
    int off6;
    unsigned char far *sm1;
    int r;
    unsigned char far *dm1;
    int h;
    int c8;
    struct Bitmap far *t7;
    int spitch;
    int pn;
    unsigned char far *dpk;

    sh5 = x & ~g_vid.mask;
    if (sh5 > 0) {
        t7 = F_1934E(src->width + 8, src->height, src->alt.p != 0L, 2);
        F_1A03F(src, sh5, t7);
    } else
        t7 = src;
    dpitch4 = dst->pitch;
    spitch = t7->pitch;
    dskip6 = dpitch4 - spitch;
    pstep5 = (dst->height - t7->height) * dpitch4;
    h = t7->height;
    off6 = y * dpitch4 + (x >> g_vid.shift);
    dpk = (unsigned char far *)dst->bits.p + off6;
    dm1 = dm = (unsigned char far *)dst->alt.p + off6;
    spn = (unsigned char far *)t7->bits.p;
    sm1 = sm = (unsigned char far *)t7->alt.p;
    for (pn = 0; pn < g_vid.planes; pn++) {
        for (r = 0; r < h; r++) {
            for (c8 = 0; c8 < spitch; c8++)
                *dpk++ |= ~*sm1++ & *spn++ & *dm1++;
            dpk += dskip6;
            dm1 += dskip6;
        }
        dpk += pstep5;
        dm1 = dm;
        sm1 = sm;
    }
    if (!g_5aa6) {
        for (r = 0, dm1 = dm, sm1 = sm; r < h; r++) {
            for (c8 = 0; c8 < spitch; c8++)
                *dm1++ &= *sm1++;
            dm1 += dskip6;
        }
    }
    if (sh5)
        F_19438(1);
}

/* F_1A69F: blit all of bm to dst at (x, y) through the blit hook */


void F_1A69F(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x;
    pkt.dy = y;
    (*g_78a2)(bm, dst, &pkt, mode);
}

/* F_1A6E0: blit to a bitmap fetched by index, remember it, advance the clock */


void F_1A6E0(struct Bitmap far *dst, int n, unsigned char x, unsigned char y)
{
    struct Bitmap far *p;

    p = F_15BBE(n, 2);
    F_1A69F(dst, p, x, y, 3);
    g_70a8 = p;
    F_19438(1);
}

/* F_1A734: draw the bitmap with index n onto dst at (x, y) */


void F_1A734(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode)
{
    struct Blt blt;
    struct Bitmap far *bm;

    bm = F_15BBE(n, 2);
    blt.w = bm->width;
    blt.h = blt.fc = bm->height;
    blt.sx = blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    F_1A2D0(bm, dst, &blt, mode);
    F_19438(1);
}
