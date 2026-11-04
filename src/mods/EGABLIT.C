#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "STRUCTS.H"
#include "EGABLIT.H"
#include "DASH.H"
#include "EGALATCH.H"
#include "MAIN.H"
#include "MASKBG.H"
#include "MASKBLT.H"
#include "MASKOR.H"
#include "PC.H"

/* egablit_copy @ 21A0:1862 (linear 0x23262, 647 bytes)

   Copy a rectangle between planar VGA bitmaps, plane by plane */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void egablit_copy(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *rect, int flags)
{
    int p;                /* source bitmap height */
    char far *a;          /* destination pointer */
    char far *b;          /* row copy: source */
    int c;                /* last plane + 1 */
    int d;                /* width */
    int e;                /* destination plane size */
    int f;                /* both planes are flat */
    int g;                /* source plane size */
    struct Blt h;
    int i;                /* source pitch */
    int j;                /* height */
    int k;                /* plane write mask */
    int L;
    int l;                /* current plane */
    int M;
    int N;
    char far *m;          /* source pointer */
    char far *n;          /* row copy: destination */
    int o;                /* first plane */

    N = (flags & 2) && dst->alt.p && src->alt.p;
    if (!dash_clip_blt(dst, src, rect, &h))
        return;
    if (flags & 1) {
        m = dst->bits.p;
        a = src->bits.p;
        o = 0;
    } else {
        m = dst->alt.p;
        a = src->alt.p;
        o = 4;
    }
    c = N ? 5 : 4;
    M = (dst->flags == VID_EGA) ? EGA_PITCH : dst->pitch;
    g = (dst->flags == VID_EGA) ? 0 : dst->height * M;
    m += h.sy * M + h.sx;
    p = src->height;
    i = (src->flags == VID_EGA) ? EGA_PITCH : src->pitch;
    e = (src->flags == VID_EGA) ? 0 : p * i;
    a += h.dy * i + h.dx;
    d = h.w;
    j = h.h;
    if (src->flags == VID_EGA && dst->flags == VID_EGA) {
        egalatch_rect(m, a, d, j, 0, 0, EGA_PITCH);
        return;
    }
    f = e == 0 || g == 0;
    if (f) {
        outp(GC_INDEX, GC_READ_MAP);
        outp(SEQ_INDEX, SEQ_MAP_MASK);
    }
    k = 1;
    for (l = o; l < c; l++, k <<= 1, m += g, a += e) {
        if (f) {
            if (g == 0)
                outp(GC_DATA, l);
            if (e == 0)
                outp(SEQ_DATA, k);
        }
        if (d == i && M == d) {
            movedata(FP_SEG(m), FP_OFF(m), FP_SEG(a), FP_OFF(a), d * j);
        } else {
            L = 0;
            b = m;
            n = a;
            for (; L < j; L++) {
                movedata(FP_SEG(b), FP_OFF(b), FP_SEG(n), FP_OFF(n), d);
                b += M;
                n += i;
            }
        }
    }
    if (f) {
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, SEQ_ALL_PLANES);
    }
}

/* egablit_sprite @ 21A0:1AE9 (linear 0x234E9, 492 bytes)

   Masked blit between planar VGA bitmaps */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void egablit_sprite(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3,
             struct Mask *mask, struct Blt *r)
{
    int p;                /* mask height */
    int a;                /* bm2 height */
    int b;                /* bm3 plane size */
    char far *c;          /* destination pointer */
    int d;                /* width */
    int e;                /* bm3 offset */
    int f;                /* bm2 plane size */
    int g;                /* bm1 pitch */
    int h;                /* bm3 pitch */
    struct Blt i;
    int j;                /* bm2 width */
    int K;                /* bm2 pitch */
    int k;                /* plane write mask */
    int L;                /* offset of the second bm1 plane */
    int l;                /* height */
    int M;                /* segment of bm1 */
    int m;                /* offset of bm1 */
    int N;                /* bm1 plane size */
    char far *n;          /* mask pointer */
    int o;                /* height below the mask */

    if (mask) {
        p = mask->h;
        n = mask->data + (r->sx >> 3);
        if (r->dx < 0)
            n += (7 - r->dx) >> 3;
    } else {
        p = 0;
        n = 0;
    }
    if (!dash_clip_blt(bm1, bm2, r, &i))
        return;
    g = bm1->pitch;
    N = bm1->height * g;
    b = bm3->pitch * bm3->height;
    d = i.w;
    l = i.h;
    o = l - p;
    j = bm2->width;
    a = bm2->height;
    K = bm2->pitch;
    h = bm3->pitch;
    f = (bm2->flags == VID_EGA) ? 0 : a * K;
    c = bm2->bits.p + i.dy * K + i.dx;
    M = bm1->bits.w.seg;
    m = bm1->bits.w.off + i.sx + i.sy * g;
    L = bm1->alt.w.off + i.sx + i.sy * g;
    if (g == h)
        e = i.dy * h + bm3->bits.w.off;
    else
        e = i.sy * h + bm3->bits.w.off + i.dx;
    k = 1;
    do {
        if (f == 0) {
            outp(SEQ_INDEX, SEQ_MAP_MASK);
            outp(SEQ_DATA, k);
        }
        maskbg_blit(c, M, m, L, e, g, d, o, h, K);
        k <<= 1;
        c += f;
        m += N;
        e += b;
    } while (k < 9);
    if (f == 0) {
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, SEQ_ALL_PLANES);
    }
    if (p > 0) {
        c += EGA_PITCH * o;
        egalatch_to_page(n, d, p, c, g);
    }
}

/* egablit_masked @ 21A0:1CD5 (linear 0x236D5, 344 bytes)

   Blit between planar VGA bitmaps through one of two hand-written plane routines */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void egablit_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag)
{
    int p;                /* bm2 height */
    char far *a;          /* destination pointer */
    int b;                /* width */
    int c;                /* bm2 plane size */
    int d;                /* bm1 pitch */
    struct Blt e;
    void (far *f)(char far *, unsigned, int, int, int, int, int, int);
    int g;                /* bm2 pitch */
    int h;                /* height */
    int i;                /* offset of the second bm1 plane */
    int j;                /* offset of bm1 */
    int k;                /* plane write mask */
    int l;                /* segment of bm1 */
    int m;                /* current plane */
    int n;                /* bm1 plane size */

    if (!dash_clip_blt(bm1, bm2, r, &e))
        return;
    d = bm1->pitch;
    b = e.w;
    h = e.h;
    n = bm1->height * d;
    p = bm2->height;
    g = (bm2->flags == VID_EGA) ? EGA_PITCH : bm2->pitch;
    c = (bm2->flags == VID_EGA) ? 0 : p * g;
    a = bm2->bits.p + e.dy * g + e.dx;
    l = bm1->bits.w.seg;
    j = bm1->bits.w.off + e.sx + e.sy * d;
    i = bm1->alt.w.off + e.sx + e.sy * d;
    f = flag ? maskor_blit : maskblt_copy;
    if (c == 0) {
        outp(GC_INDEX, GC_READ_MAP);
        outp(SEQ_INDEX, SEQ_MAP_MASK);
    }
    k = 1;
    m = 0;
    for (; k < 9; k <<= 1, m++, j += n, a += c) {
        if (c == 0) {
            outp(GC_DATA, m);
            outp(SEQ_DATA, k);
        }
        (*f)(a, l, j, i, d, b, h, g);
    }
    if (c == 0) {
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, SEQ_ALL_PLANES);
    }
}

/* The file is compiled /Oals, but this copy loop is a plain /Ol-off loop in the
   shipped code (byte moves through memory variables). Under /Ol MSC turns the
   inner loop into rep movsw, and every for/while/do form gets registers and
   loop temps. A word move would also break the write-mode-1 latch copy (the
   second byte's latches are written to both bytes), a likely reason the
   programmer switched the loop optimizer off here. */
#pragma loop_opt(off)

/* egablit_latch_pack @ 21A0:1E2D (linear 0x2382D, 117 bytes)

   Copy a w by h block of planar VGA video memory using write mode 1 (latch copy) */

void egablit_latch_pack(char far *src, int w, int h, char far *dst, int pitch)
{
    char far *p;
    int row;
    int col;

    if (h >= 1) {
        outp(GC_INDEX, GC_MODE);
        outp(GC_DATA, GC_WRITE_MODE1);
        for (row = 0; row < h; row++, src += pitch) {
            for (col = 0, p = src; col < w; col++)
                *dst++ = *p++;
        }
        outp(GC_INDEX, GC_MODE);
        outp(GC_DATA, GC_WRITE_MODE0);
    }
}
