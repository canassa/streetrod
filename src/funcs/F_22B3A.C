/* F_22B3A: draw a string in the 8-pixel-high font into a planar EGA/VGA bitmap, composing the glyph rows in a 320 byte buffer first */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp, memset, _rotr)

void F_22B3A(struct Bitmap far *bm, int x, int y, char *str)
{
    int cols;
    unsigned char far *q;
    unsigned char bits[320];
    unsigned char far *r;
    unsigned char far *pen;
    int c, d;
    unsigned char byte, e;
    int g;
    unsigned char far *fill;
    int part;
    char i;
    int k, sh;
    unsigned char ch;
    int left, wd, l, m;
    unsigned char o;

    d = x & 7;
    e = 0xff;
    o = 0xff;
    i = bm->flags <= -1;
    memset(bits, 0, 320);
    cols = sh = d;
    q = bits;
    /* a for with an increment (here one with no code) gives the original layout: jmp test, the tail of the body, then the test with the body head */
    for (; *str; str) {
        byte = *str++ - 0x20;
        if (byte > 0x5a)
            byte = 0;
        wd = *(pen = (unsigned char far *)&g_font[byte]);
        cols += wd;
        if (cols > 320) {
            cols -= wd;
            break;
        }
        g = sh + wd;
        if (g <= 8) {
            r = q;
            for (left = 8; left--; r += 0x28)
                *r |= *++pen >> sh;
            sh = g;
            if (sh == 8) {
                *++q = 0;
                sh = 0;
            }
        } else {
            q[1] = 0;
            r = q;
            for (left = 8; left--; r += 0x28)
                *(unsigned far *)r |= _rotr(*++pen, sh);
            sh = g & 7;
            q++;
        }
    }
    if (x + cols - d > bm->width)
        cols = bm->width - x;
    c = (d + 7) & ~7;
    m = cols & ~7;
    e >>= d - c + 8;
    o <<= m - cols + 8;
    k = (c < m) ? (m - c) >> 3 : 0;
    if (k == 0 && d >= m) {
        g = e & o;
        if (g) {
            e = g;
            o = 0;
        }
    }
    if (y + (part = 8) > bm->height)
        part = bm->height - y;
    g_824c = x + cols - d;
    g_824e = y;
    outp(0x3ce, 5);
    outp(0x3cf, 2);
    outp(0x3c4, 2);
    outp(0x3c5, 0xf);
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    g = part;
    fill = pen = bits;
    r = q = bm->bits.p + bm->pitch * y + (x >> 3);
    l = bm->pitch;
    for (; g != 0; g--, q += l, r = q, fill = pen += 0x28) {
        if (e) {
            ch = *r;
            outp(0x3cf, e);
            *r = g_colors.c[1];
            ch = *r;
            outp(0x3cf, *fill);
            fill++;
            *r = g_colors.c[0];
            r++;
        }
        for (left = k; left != 0; left--) {
            outp(0x3cf, 0xff);
            *r = g_colors.c[1];
            ch = *r;
            outp(0x3cf, *fill);
            fill++;
            *r = g_colors.c[0];
            r++;
        }
        if (o) {
            ch = *r;
            outp(0x3cf, o);
            *r = g_colors.c[1];
            ch = *r;
            outp(0x3cf, *fill);
            fill++;
            *r = g_colors.c[0];
        }
    }
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    outp(0x3ce, 5);
    outp(0x3cf, 0);
}
