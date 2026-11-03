/* F_1C803: fill a rectangle of 2-bit CGA pixels with a colour, recolouring every pixel that is not colour c1l */

#include <memory.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

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
