#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "EGA.H"
#include "CURSBLT.H"
#include "DASH.H"
#include "EGABLIT.H"
#include "EGACOPY.H"
#include "EGALINE.H"
#include "EGARECOL.H"
#include "EGASHIFT.H"
#include "EGASLIDE.H"
#include "EGASPAN.H"
#include "EGASPR.H"
#include "FONTDATA.H"
#include "MENU.H"
#include "PC.H"
#include "SCREEN.H"
#include "VIDEO.H"

void (far *g_egaHooks[15])() = {egablit_copy, egashift_blit, egablit_masked, ega_draw_text, cursblt_ega, egacopy_page, egarecol_rect, dash_pal_blank, dash_pal_show, egaspr_step, egacopy_reveal, egaslide_sprite, egaline_draw, egaspan_fill, egaspan_fill_below};   /* DS:5BD0 */

/* ega_install @ 21A0:1128 (linear 0x22B28, 18 bytes) */

void ega_install(void)
{
    *(struct Hooks *)&b_78a2 = *(struct Hooks *)g_egaHooks;
}

/* ega_draw_text @ 21A0:113A (linear 0x22B3A, 973 bytes)

   Draw a string in the 8-pixel-high font into a planar EGA/VGA bitmap, composing the glyph rows in
   a 320 byte buffer first */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void ega_draw_text(struct Bitmap far *bm, int x, int y, char *str)
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
        if (cols > SCREEN_WIDTH) {
            cols -= wd;
            break;
        }
        g = sh + wd;
        if (g <= 8) {
            r = q;
            for (left = 8; left--; r += EGA_PITCH)
                *r |= *++pen >> sh;
            sh = g;
            if (sh == 8) {
                *++q = 0;
                sh = 0;
            }
        } else {
            q[1] = 0;
            r = q;
            for (left = 8; left--; r += EGA_PITCH)
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
    g824cd = x + cols - d;
    bq824e = y;
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE2);
    outp(SEQ_INDEX, SEQ_MAP_MASK);
    outp(SEQ_DATA, SEQ_ALL_PLANES);
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    g = part;
    fill = pen = bits;
    r = q = bm->bits.p + bm->pitch * y + (x >> 3);
    l = bm->pitch;
    for (; g != 0; g--, q += l, r = q, fill = pen += EGA_PITCH) {
        if (e) {
            ch = *r;
            outp(GC_DATA, e);
            *r = x_colors.c[1];
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.c[0];
            r++;
        }
        for (left = k; left != 0; left--) {
            outp(GC_DATA, GC_MASK_ALL);
            *r = x_colors.c[1];
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.c[0];
            r++;
        }
        if (o) {
            ch = *r;
            outp(GC_DATA, o);
            *r = x_colors.c[1];
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.c[0];
        }
    }
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE0);
}
