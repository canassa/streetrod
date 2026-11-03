#include <memory.h>
#include <dos.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <conio.h>

/* F_1C2CA: draw a string of 8-pixel-high CGA/Hercules text into a bitmap at pixel x, y, through an 8x80 byte scratch buffer */


#pragma intrinsic(_rotl)

unsigned char g_5b40[4] = {192, 48, 12, 3};   /* DS:5B40 */
unsigned char g_5b44[4] = {63, 207, 243, 252};   /* DS:5B44 */
int g_5b48[5] = {0, 2, 4, 1, 3};   /* DS:5B48 */

void F_1C2CA(struct Bitmap far *bm, int x, int y, char *s)
{
    unsigned seg1;
    int cga5;
    unsigned char *rp;
    int nb2;
    int col4;
    unsigned *pp2;
    unsigned char far *b0;
    unsigned char lm28;
    unsigned *p3;
    unsigned char far *b1;
    unsigned char far *s0;
    unsigned fg7;
    int t;
    unsigned buf7[8][40];
    unsigned char *from;
    unsigned char far *s1;
    unsigned char *fill;
    unsigned dseg4;
    int shl;
    unsigned far *gp;
    int w169;
    int c4;
    unsigned bgn;
    int wid4;
    int r6;
    int h;
    unsigned char lmask;
    int i;
    int bitoff1;
    unsigned off0;
    int rb8;
    unsigned off1;
    unsigned char rmask;
    unsigned seg0;

    shl = (x & 3) << 1;
    lmask = 0xff;
    rmask = 0xff;
    memset(buf7, 0, sizeof buf7);
    if (g_vid.mode == -4)
        g_colors.c[0] = 0;
    t = g_colors.c[0];
    fg7 = (t << 2) | (t << 4) | (t << 6) | t;
    fg7 |= fg7 << 8;
    t = g_colors.c[1];
    bgn = (t << 2) | (t << 4) | (t << 6) | t;
    bgn |= bgn << 8;
    col4 = bitoff1 = shl;
    pp2 = &buf7[0][0];
    while (*s) {
        if ((c4 = *s++ - 0x20) < 0 || c4 > 0x5a)
            c4 = 0;
        col4 += (wid4 = *(gp = &g_cgafont[c4].w) * 2);
        if (col4 >= 0x280) {
            col4 -= wid4;
            break;
        }
        r6 = bitoff1 + wid4;
        if (r6 <= 16) {
            p3 = pp2;
            i = 8;
            while (i--) {
                *p3 |= *++gp >> bitoff1;
                p3 += 40;
            }
            bitoff1 = r6;
            if (r6 == 16) {
                pp2++;
                bitoff1 = 0;
            }
        } else {
            p3 = pp2;
            i = 8;
            while (i--) {
                *p3 |= *++gp >> bitoff1;
                p3[1] |= *gp << (16 - bitoff1);
                p3 += 40;
            }
            bitoff1 = r6 & 15;
            pp2++;
        }
    }
    g_824c = ((col4 - shl) >> 1) + x;
    g_824e = y;
    if (col4 == shl)
        return;
    if (shl) {
        lmask <<= 8 - shl;
        lm28 = ~lmask;
    }
    rb8 = col4 & 7;
    if (rb8)
        rmask >>= rb8;
    nb2 = col4 >> 3;
    w169 = nb2;
    if (rb8)
        w169++;
    if (w169 & 1)
        w169++;
    w169 >>= 1;
    if (shl)
        nb2--;
    if (nb2 < 0) {
        lmask |= rmask;
        rb8 = 0;
    }
    if (y + (h = 8) > bm->height)
        h = bm->height - y;
    r6 = 0;
    pp2 = &buf7[0][0];
    for (; r6 < h; r6++) {
        for (i = 0, p3 = pp2; i < w169; i++) {
            *p3 = (~(*p3 = _rotl(*p3, 8)) & bgn) | (*p3 & fg7);
            p3++;
        }
        pp2 += 40;
    }
    b0 = b1 = bm->bits.p;
    cga5 = (g_vid.mode == -4 ? 0xb000 : 0xb800) == FP_SEG(b0);
    b1 += bm->planesize >> 1;
    b0 += bm->pitch * (y >> 1);
    b0 += x >> 2;
    b1 += bm->pitch * (y >> 1);
    b1 += x >> 2;
    rp = (unsigned char *)buf7;
    if (shl) {
        r6 = 0;
        s1 = b1;
        s0 = b0;
        fill = rp;
        for (; r6 < h; r6++, fill += 80) {
            if ((r6 + y) & 1) {
                *s1 &= lmask;
                *s1 |= *fill & lm28;
                if (cga5 && g_vid.mode == -4)
                    s1[0x2000] = *s1;
                s1 += bm->pitch;
                s0 += bm->pitch;
            } else {
                *s0 &= lmask;
                *s0 |= *fill & lm28;
            }
        }
        b0++;
        b1++;
        rp++;
    }
    if (nb2 > 0) {
        dseg4 = data_segment();
        seg0 = FP_SEG(b0);
        seg1 = FP_SEG(b1);
        off0 = FP_OFF(b0);
        off1 = FP_OFF(b1);
        from = rp;
        for (i = 0; i < h; i++, from += 80) {
            if ((y + i) & 1) {
                movedata(dseg4, (unsigned)from, seg1, off1, nb2);
                if (cga5 && g_vid.mode == -4)
                    movedata(dseg4, (unsigned)from, seg1, off1 + 0x2000, nb2);
                off0 += 80;
                off1 += 80;
            } else
                movedata(dseg4, (unsigned)from, seg0, off0, nb2);
        }
        b0 += nb2;
        b1 += nb2;
        rp += nb2;
    }
    if (rb8) {
        r6 = 0;
        s1 = b1;
        s0 = b0;
        fill = rp;
        for (; r6 < h; r6++, fill += 80) {
            if ((r6 + y) & 1) {
                *s1 &= rmask;
                *s1 |= *fill;
                if (cga5 && g_vid.mode == -4)
                    s1[0x2000] = *s1;
                s1 += bm->pitch;
                s0 += bm->pitch;
            } else {
                *s0 &= rmask;
                *s0 |= *fill;
            }
        }
    }
}

/* F_1C803: fill a rectangle of 2-bit CGA pixels with a colour, recolouring every pixel that is not colour c1l */


#define PIX(k, cv, bv) \
    if ((*p5 & g_5b40[k]) != (cv)) { *p5 &= g_5b44[k]; *p5 |= (bv); }

#define EDGES_LEFT \
    if (lx) { \
        if (lx <= 1) PIX(1, c4z, b4k) \
        if (lx <= 2) { \
            if (nmid8 < 0 && (rx == 1 || rx == 2)) return; \
            PIX(2, c2, b2) \
        } \
        if (nmid8 < 0 && rx != 0) return; \
        PIX(3, cc, bb1) \
        p5++; \
    }

#define EDGES_RIGHT \
    if (rx != 0 && nmid8 >= 0) { \
        PIX(0, c67, b68) \
        if (rx <= 1) continue; \
        PIX(1, c4z, b4k) \
        if (rx <= 2) continue; \
        PIX(2, c2, b2) \
    }

void F_1C803(struct Bitmap far *bm, int a, int b, int x1, int y1, int x2, int y2)
{
    int nmid8;
    char far *p0;
    unsigned char c67, b68;
    char c1l;
    char far *p1;
    unsigned char c4z, b4k, b2;
    int lx;
    unsigned char bb1;
    char far *p5;
    unsigned char c2, cc, w2;
    int i, y, v4, rx;

    c1l = g_colors.c[1];
    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (x2 > 0x13f)
        x2 = 0x140;
    if (y2 > 0xc7)
        y2 = 0xc7;
    p0 = p1 = bm->bits.p;
    p1 += bm->planesize >> 1;
    p0 += bm->pitch * (y1 >> 1);
    p0 += x1 >> 2;
    p1 += bm->pitch * (y1 >> 1);
    p1 += x1 >> 2;
    nmid8 = (x2 >> 2) - ((x1 + 3) >> 2);
    lx = x1 & 3;
    rx = x2 & 3;
    bb1 = b;
    b68 = bb1 << 6;
    b4k = bb1 << 4;
    b2 = bb1 << 2;
    cc = c1l;
    c67 = cc << 6;
    c4z = cc << 4;
    c2 = cc << 2;
    if (c1l + a != 3) {
        for (y = y1; y < y2; y++) {
            if (y & 1) {
                p5 = p1;
                p1 += 0x50;
                p0 += 0x50;
            } else
                p5 = p0;
            EDGES_LEFT
            for (i = 0; i < nmid8; i++) {
                PIX(0, c67, b68)
                PIX(1, c4z, b4k)
                PIX(2, c2, b2)
                PIX(3, cc, bb1)
                p5++;
            }
            EDGES_RIGHT
        }
    } else {
        if (c1l == 0 && b == 1) {
            v4 = 0x26;
            w2 = 0x55;
        } else if (c1l == 0 && b == 2) {
            v4 = 0x26;
            w2 = 0xaa;
        } else if (c1l == 1 && b == 0) {
            v4 = 0x26;
            w2 = 0x55;
        } else if (c1l == 1 && b == 3) {
            v4 = 0x7c;
            w2 = 0x55;
        } else if (c1l == 2 && b == 0) {
            v4 = 0x26;
            w2 = 0xaa;
        } else if (c1l == 2 && b == 3) {
            v4 = 0x7c;
            w2 = 0xaa;
        } else if (c1l == 3 && b == 1) {
            v4 = 0x7c;
            w2 = 0x55;
        } else if (c1l == 3 && b == 2) {
            v4 = 0x7c;
            w2 = 0xaa;
        }
        if (v4 == 0x26) {
            for (y = y1; y < y2; y++) {
                if (y & 1) {
                    p5 = p1;
                    p1 += 0x50;
                    p0 += 0x50;
                } else
                    p5 = p0;
                EDGES_LEFT
                for (i = 0; i < nmid8; i++)
                    *p5++ |= w2;
                EDGES_RIGHT
            }
        } else {
            for (y = y1; y < y2; y++) {
                if (y & 1) {
                    p5 = p1;
                    p1 += 0x50;
                    p0 += 0x50;
                } else
                    p5 = p0;
                EDGES_LEFT
                for (i = 0; i < nmid8; i++)
                    *p5++ |= w2;
                EDGES_RIGHT
            }
        }
    }
}

/* F_1CEE7: copy a 16K video page, and on Hercules also its fourth bank */


void F_1CEE7(char far *src, char far *dst)
{
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), 0x4000);
    if (g_vid.mode == -4 && (g_vid.mode == -4 ? 0xb000 : 0xb800) == FP_SEG(dst))
        movedata(g_vid.mode == -4 ? 0xb200 : 0xba00, 0, 0xb400, 0, 0x2000);
}

/* F_1CF40: dissolve one 80-byte-per-line CGA screen into another, five interleaved passes */


void F_1CF40(char far *dst, char far *src)
{
    char far *d;
    char far *s2;
    int far *e;
    long deadline;
    int pass;
    int y;
    int k;

    for (pass = 0; pass < 5; pass++) {
        deadline = g_tick + 5;
        for (y = g_5b48[pass]; y < 200; y += 5) {
            d = dst + (y >> 1) * 0x50;
            s2 = src + (y >> 1) * 0x50;
            if (y & 1) {
                d += 0x2000;
                s2 += 0x2000;
            }
            for (e = (int far *)d, k = 0x28; k; k--)
                *e++ = 0;
            movedata(FP_SEG(s2), FP_OFF(s2), FP_SEG(d), FP_OFF(d), 0x50);
            if ((y & 1) && g_vid.mode == -4) {
                d += 0x2000;
                for (e = (int far *)d, k = 0x28; k; k--)
                    *e++ = 0;
                movedata(FP_SEG(s2), FP_OFF(s2), FP_SEG(d), FP_OFF(d), 0x50);
            }
        }
        while (g_tick < deadline)
            ;
    }
}

/* F_1D05B: BIOS video function 5, select the active display page */


void F_1D05B(int page)
{
    union REGS r;

    r.h.ah = 5;
    r.h.al = page;
    int86(0x10, &r, &r);
}

#pragma intrinsic(outp)
/* Recovered unreferenced video-control routine at 0x1D080. */
void F_1D080(void)
{
    switch (g_vid.mode) {
    case -3:
        outp(0x3d8, 2);
        break;
    case -4:
        outp(0x3b8, 0x22);
        break;
    }
    g_8249 = 0;
}

#pragma intrinsic(outp)
/* Recovered unreferenced video-control routine at 0x1D0A4. */
void F_1D0A4(void)
{
    switch (g_vid.mode) {
    case -3:
        outp(0x3d8, 0x2a);
        break;
    case -4:
        outp(0x3b8, 0x2a);
        break;
    }
    g_8249 = 1;
}
