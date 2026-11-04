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
#include "PC.H"
#include "SCREEN.H"
#include "VIDEO.H"

struct Hooks g_egaHooks = {eb_copy,     ex_blit, eb_masked, eg_draw_text, uc_ega,  ec_page, er_rect,      da_pal_blank,
                           da_pal_show, es_step, ec_reveal, ev_sprite,    el_draw, en_fill, en_fill_below};


void eg_install(void)
{
    g_videoOps = g_egaHooks;
}

/* Draw a string in the 8-pixel-high font into a planar EGA/VGA bitmap, composing the glyph rows in
   a 320 byte buffer first */

void eg_draw_text(struct Bitmap far *bm, int x, int y, char *str)
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
    g_penPoint.x = x + cols - d;
    g_penPoint.y = y;
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
            *r = x_colors.textBg;
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.text;
            r++;
        }
        for (left = k; left != 0; left--) {
            outp(GC_DATA, GC_MASK_ALL);
            *r = x_colors.textBg;
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.text;
            r++;
        }
        if (o) {
            ch = *r;
            outp(GC_DATA, o);
            *r = x_colors.textBg;
            ch = *r;
            outp(GC_DATA, *fill);
            fill++;
            *r = x_colors.text;
        }
    }
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE0);
}
