#include "STRUCTS.H"
#include "M_13682.H"
#include "M_0F38A.H"
#include "M_10B7C.H"

/* F_13682: fill the rectangle (x, y, w, h) of bitmap s with colour x_colors.c[2], clipped to its size,
   one horizontal line at a time through the line routine a_78d2 */

void F_13682(struct Bitmap far *s, int x, int y, int w, int h)
{
    int col;
    unsigned oldseg;
    int j;
    unsigned oldoff;
    int x2;

    if (x + w > s->width)
        w = s->width - x;
    if (y + h > s->height)
        h = s->height - y;
    oldseg = av787at;
    oldoff = at767ct;
    av787at = s->bits.w.seg;
    at767ct = s->bits.w.off;
    col = x_colors.c[2];
    x2 = x + w - 1;
    for (j = 0; j < h; j++)
        (*a_78d2)(x, y + j, x2, y + j, col);
    av787at = oldseg;
    at767ct = oldoff;
}

/* F_1371C: vertical line of height h at (x, y) in bitmap s with colour x_colors.c[3], clipped to the bitmap */

void F_1371C(struct Bitmap far *s, int x, int y, int h)
{
    unsigned oldseg;
    unsigned oldoff;

    if (h + y > s->height)
        h = s->height - y;
    oldseg = av787at;
    oldoff = at767ct;
    av787at = s->bits.w.seg;
    at767ct = s->bits.w.off;
    (*a_78d2)(x, y, x, y + h - 1, x_colors.c[3]);
    av787at = oldseg;
    at767ct = oldoff;
}

/* F_1377F: horizontal line of width w at (x, y) in bitmap s, drawn with colour x_colors.c[3] */

void F_1377F(struct Bitmap far *s, int x, int y, int w)
{
    char old;

    old = x_colors.c[2];
    x_colors.c[2] = x_colors.c[3];
    F_13682(s, x, y, w, 1);
    x_colors.c[2] = old;
}

/* F_137B2: draw a raised box frame of width 3 around the rectangle (x, y, w, h) of bitmap s: four
   filled bars in colour x_colors.c[2] (x_colors.c[3] when the style in x_colors.c[4] is 3), then an inner
   highlight made of two vertical and two horizontal lines, doubled when the style is 2 */

void F_137B2(struct Bitmap far *s, int x, int y, int w, int h)
{
    int bm = x_colors.c[4] == 2 ? 3 : 3;
    int bh6;
    int right2;
    int bottom;
    int bw;
    int top8;
    int left;
    char old;

    right2 = x + w;
    left = x - bm;
    bottom = y + h;
    top8 = y - bm;
    bh6 = h + bm * 2;
    bw = w + bm * 2;
    old = x_colors.c[2];
    if (x_colors.c[4] == 3)
        x_colors.c[2] = x_colors.c[3];
    F_13682(s, left, top8, bm, bh6);
    F_13682(s, x, bottom, w, bm);
    F_13682(s, right2, top8, bm, bh6);
    F_13682(s, x, top8, w, bm);
    if (x_colors.c[4] == 3)
        x_colors.c[2] = old;
    else {
        F_1371C(s, left + 1, top8 + 1, bh6 - 2);
        F_1377F(s, left + 1, bottom + 1, bw - 2);
        F_1371C(s, right2 + 1, top8 + 1, bh6 - 2);
        F_1377F(s, left + 1, top8 + 1, bw - 2);
        if (x_colors.c[4] == 2) {
            F_1371C(s, left + 2, top8 + 2, bh6 - 4);
            F_1377F(s, left + 2, bottom, bw - 4);
            F_1371C(s, right2, top8 + 2, bh6 - 4);
            F_1377F(s, left + 2, top8 + 2, bw - 4);
        }
    }
}

/* F_13957: draw a frame around the rectangle (x, y, w, h) of bitmap s, one pixel outside it:
   two vertical lines and two horizontal ones */

void F_13957(struct Bitmap far *s, int x, int y, int w, int h)
{
    int x1;
    int x2;
    int y1;
    int y2;

    x1 = x - 1;
    x2 = x + w;
    y1 = y - 1;
    y2 = y + h;
    F_1371C(s, x1, y1, h + 2);
    F_1377F(s, x, y2, w);
    F_1371C(s, x2, y1, h + 2);
    F_1377F(s, x, y1, w);
}
