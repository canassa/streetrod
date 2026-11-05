#include <memory.h>
#include <dos.h>
#include "BITMAP.H"
#include "ARENA.H"
#include "CARHOP.H"
#include "CARPIC.H"
#include "DASH.H"
#include "DISK.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "PC.H"

/* Copy a bitmap into another through the blit hook */

void bm_copy(struct Bitmap far *src, struct Bitmap far *dst, int e)
{
    struct Blt pkt;

    me_new_bitmap(src->width, src->height, dst->mask.p ? 1 : 0, BITMAP_WORK, dst);
    pkt.w = src->width;
    pkt.h = src->height;
    pkt.dx = pkt.dy = pkt.sx = pkt.sy = 0;
    (*g_videoOps.copy)(src, dst, &pkt, e);
}

/* Find the first and last set pixel column of a bitmap */

void bm_opaque_span(struct Bitmap far *bm, int *left, int *right)
{
    int a, f, d;
    unsigned char c, e, b;
    char far *g, far *h;

    a = bm->pitch;
    f = bm->height;
    h = (g = bm->mask.p) + a - 1;
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
    *left = d / r_vid.planeBitsPerPixel;
    for (d = 0, b = 1; d < 8; d++, b <<= 1)
        if (!(e & b))
            break;
    *right = (bm->pitch << r_vid.shift) - 1;
    *right -= d / r_vid.planeBitsPerPixel;
}

/* Shift a bitmap (pixels and mask) right by n pixels into a new, wider bitmap */

struct Bitmap far *bm_shift_right(struct Bitmap far *src, int n, struct Bitmap far *dst)
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

    shl5 = (n * r_vid.planeBitsPerPixel) & 7;
    compl= 8 - shl5;
    mask = 0xff >> compl;
    h3 = src->height;
    pitch = src->pitch;
    if (n <= 0) {
        bm_copy(src, dst, src->mask.p ? 3 : 1);
        return dst;
    }
    acc1 = 0xff;
    sp8 = (unsigned char far *)src->mask.p + pitch - 1;
    for (j2 = 0; j2 < h3; j2++) {
        acc1 &= *sp8;
        sp8 += pitch;
    }
    extra8 = (acc1 & mask) == mask ? 0 : 1 << r_vid.shift;
    out = me_new_bitmap(src->width + extra8, src->height, src->mask.p ? 1 : 0, BITMAP_WORK, dst);
    dp8 = (unsigned char far *)out->bits.p;
    sp8 = (unsigned char far *)src->bits.p;
    for (k = 0; k < h3 * r_vid.planes; k++) {
        for (i21 = 0, acc1 = 0; i21 < pitch; i21++) {
            *dp8++ = (*sp8 >> shl5) | (acc1 << compl);
            acc1 = mask & *sp8;
            sp8++;
        }
        if (extra8)
            *dp8++ = acc1 << compl;
    }
    if (src->mask.p) {
        dp25 = (unsigned char far *)out->mask.p;
        sp24 = (unsigned char far *)src->mask.p;
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

/* Mask-blit src into dst, optionally and-ing the pixels together */

void bm_draw_masked(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag)
{
    struct Blt a;
    struct Blt e;
    struct Bitmap far *j;
    char far *c;
    char far *g;
    int f, b, h, d, i, k, l, m;

    for (i = 0; i < 10; i++)
        ((int *)&e)[i] = ((int *)r)[i];
    f = r->dx & ~r_vid.xByteMask;
    if (f > 0) {
        j = ar_bm(dst->width + 8, dst->height, dst->mask.p != 0L, ARENA_HIGH_STACK);
        bm_shift_right(dst, f, j);
        e.w = j->width;
    } else
        j = dst;
    (*g_videoOps.maskedBlit)(j, src, &e, 1);
    if (flag) {
        if (da_clip_blt(j, src, &e, &a)) {
            d = j->pitch;
            b = a.w;
            h = a.h;
            l = d - b;
            m = src->pitch - b;
            c = j->mask.p + a.sy * d + a.sx;
            g = src->mask.p + src->pitch * a.dy + a.dx;
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
        ma_pop_high(1);
}

/* Draw a masked bitmap into another at pixel (x, y), pre-shifting it to the byte phase */

void bm_underlay(struct Bitmap far *src, struct Bitmap far *dst, int x, int y)
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

    sh5 = x & ~r_vid.xByteMask;
    if (sh5 > 0) {
        t7 = ar_bm(src->width + 8, src->height, src->mask.p != 0L, ARENA_HIGH_STACK);
        bm_shift_right(src, sh5, t7);
    } else
        t7 = src;
    dpitch4 = dst->pitch;
    spitch = t7->pitch;
    dskip6 = dpitch4 - spitch;
    pstep5 = (dst->height - t7->height) * dpitch4;
    h = t7->height;
    off6 = y * dpitch4 + (x >> r_vid.shift);
    dpk = (unsigned char far *)dst->bits.p + off6;
    dm1 = dm = (unsigned char far *)dst->mask.p + off6;
    spn = (unsigned char far *)t7->bits.p;
    sm1 = sm = (unsigned char far *)t7->mask.p;
    for (pn = 0; pn < r_vid.planes; pn++) {
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
    if (!g_keepMask) {
        for (r = 0, dm1 = dm, sm1 = sm; r < h; r++) {
            for (c8 = 0; c8 < spitch; c8++)
                *dm1++ &= *sm1++;
            dm1 += dskip6;
        }
    }
    if (sh5)
        ma_pop_high(1);
}

/* Blit all of bm to dst at (x, y) through the blit hook */

void bm_put(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x;
    pkt.dy = y;
    (*g_videoOps.copy)(bm, dst, &pkt, mode);
}

/* Blit to a bitmap fetched by index, remember it, advance the clock */

void bm_put_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y)
{
    struct Bitmap far *p;

    p = di_bm(n, ARENA_HIGH_STACK);
    bm_put(dst, p, x, y, 3);
    g_lastPic = p;
    ma_pop_high(1);
}

/* Draw the bitmap with index n onto dst at (x, y) */

void bm_draw_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode)
{
    struct Blt blt;
    struct Bitmap far *bm;

    bm = di_bm(n, ARENA_HIGH_STACK);
    blt.w = bm->width;
    blt.h = blt.unused1 = bm->height;
    blt.sx = blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    bm_draw_masked(bm, dst, &blt, mode);
    ma_pop_high(1);
}

/* And one bitmap rectangle into another */

void bm_and_mask(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in)
{
    struct Blt d;
    char far *a;
    char far *h;
    int b, c, e, f, g, i;

    if (da_clip_blt(src, dst, in, &d)) {
        c = d.w;
        e = d.h;
        b = src->pitch - c;
        i = dst->pitch - c;
        h = src->mask.p + src->pitch * d.sy + d.sx;
        a = dst->mask.p + dst->pitch * d.dy + d.dx;
        for (f = 0; f < e; f++) {
            for (g = 0; g < c; g++, a++, h++)
                *a &= *h;
            h += b;
            a += i;
        }
    }
}

/* Far memory copy */

void bm_far_copy(char far *src, char far *dst, unsigned n)
{
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), n);
}

/* Grab the w x h area at (sx, sy) of a bitmap into a new bitmap */

struct Bitmap far *bm_grab(struct Bitmap far *bm, struct Blt *r, int mode)
{
    struct Blt a;
    int b, c, d, e;
    struct Bitmap far *f;

    b = r->w;
    d = r->h;
    c = r->sx;
    e = r->sy;
    f = ar_bm(b, d, bm->mask.p != 0L, mode);
    a.w = b;
    a.h = d;
    a.sx = c;
    a.sy = e;
    a.dx = a.dy = 0;
    (*g_videoOps.copy)(bm, f, &a, 3);
    return f;
}

/* Copy a screen rectangle between two screens, clipped to 320x200 */

void bm_copy_box(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2)
{
    struct Blt pkt;

    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (x2 > (SCREEN_WIDTH - 1))
        x2 = (SCREEN_WIDTH - 1);
    if (y2 > (SCREEN_HEIGHT - 1))
        y2 = (SCREEN_HEIGHT - 1);
    pkt.w = ((x2 + ~r_vid.xByteMask + 1) & r_vid.xByteMask) - (x1 & r_vid.xByteMask);
    pkt.h = y2 - y1 + 1;
    pkt.sx = pkt.dx = x1;
    pkt.sy = pkt.dy = y1;
    (*g_videoOps.copy)(dst, src, &pkt, 1);
}
