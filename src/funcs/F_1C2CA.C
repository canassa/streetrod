/* F_1C2CA: draw a string of 8-pixel-high CGA/Hercules text into a bitmap at pixel x, y, through an 8x80 byte scratch buffer */

#include <memory.h>
#include <dos.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(_rotl)

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
    unsigned char *qd;
    unsigned char far *s1;
    unsigned char *qs;
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
    bitoff1 = col4 = shl;
    pp2 = &buf7[0][0];
    while (*s) {
        c4 = *s++ - 0x20;
        if (c4 < 0 || c4 > 0x5a)
            c4 = 0;
        gp = &g_cgafont.g[c4].w;
        col4 += (wid4 = *gp * 2);
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
                ++gp;
                *p3 |= *gp >> bitoff1;
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
    for (r6 = 0; r6 < h; r6++) {
        for (i = 0, p3 = pp2; i < w169; i++) {
            *p3 = (~(*p3 = _rotl(*p3, 8)) & bgn) | (*p3 & fg7);
            p3++;
        }
        pp2 += 40;
    }
    b1 = b0 = bm->bits.p;
    cga5 = (g_vid.mode == -4 ? 0xb000 : 0xb800) == FP_SEG(b0);
    b1 += bm->planesize >> 1;
    b0 += bm->pitch * (y >> 1);
    b0 += x >> 2;
    b1 += bm->pitch * (y >> 1);
    b1 += x >> 2;
    rp = (unsigned char *)buf7;
    if (shl) {
        s1 = b1;
        s0 = b0;
        qs = rp;
        for (r6 = 0; r6 < h; r6++, qs += 80) {
            if ((r6 + y) & 1) {
                *s1 &= lmask;
                *s1 |= *qs & lm28;
                if (cga5 && g_vid.mode == -4)
                    s1[0x2000] = *s1;
                s1 += bm->pitch;
                s0 += bm->pitch;
            } else {
                *s0 &= lmask;
                *s0 |= *qs & lm28;
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
        qd = rp;
        for (r6 = 0; r6 < h; r6++, qd += 80) {
            if ((r6 + y) & 1) {
                movedata(dseg4, (unsigned)qd, seg1, off1, nb2);
                if (cga5 && g_vid.mode == -4)
                    movedata(dseg4, (unsigned)qd, seg1, off1 + 0x2000, nb2);
                off0 += 80;
                off1 += 80;
            } else
                movedata(dseg4, (unsigned)qd, seg0, off0, nb2);
        }
        b0 += nb2;
        b1 += nb2;
        rp += nb2;
    }
    if (rb8) {
        s1 = b1;
        s0 = b0;
        qs = rp;
        for (r6 = 0; r6 < h; r6++, qs += 80) {
            if ((r6 + y) & 1) {
                *s1 &= rmask;
                *s1 |= *qs;
                if (cga5 && g_vid.mode == -4)
                    s1[0x2000] = *s1;
                s1 += bm->pitch;
                s0 += bm->pitch;
            } else {
                *s0 &= rmask;
                *s0 |= *qs;
            }
        }
    }
}
