#include "STRUCTS.H"
#include "DRAW.H"
#include "DASH.H"
#include "VIDEO.H"

/* dr_fill_rect @ 0F38:4302 (linear 0x13682, 154 bytes)

   Fill the rectangle (x, y, w, h) of bitmap s with colour x_colors.fill, clipped to its size,
   one horizontal line at a time through the line routine g_videoOps.drawLine */

void dr_fill_rect(struct Bitmap far *s, int x, int y, int w, int h)
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
    col = x_colors.fill;
    x2 = x + w - 1;
    for (j = 0; j < h; j++)
        (*g_videoOps.drawLine)(x, y + j, x2, y + j, col);
    av787at = oldseg;
    at767ct = oldoff;
}

/* dr_vline @ 0F38:439C (linear 0x1371C, 99 bytes)

   Vertical line of height h at (x, y) in bitmap s with colour x_colors.line, clipped to the bitmap */

void dr_vline(struct Bitmap far *s, int x, int y, int h)
{
    unsigned oldseg;
    unsigned oldoff;

    if (h + y > s->height)
        h = s->height - y;
    oldseg = av787at;
    oldoff = at767ct;
    av787at = s->bits.w.seg;
    at767ct = s->bits.w.off;
    (*g_videoOps.drawLine)(x, y, x, y + h - 1, x_colors.line);
    av787at = oldseg;
    at767ct = oldoff;
}

/* dr_hline @ 0F38:43FF (linear 0x1377F, 51 bytes)

   Horizontal line of width w at (x, y) in bitmap s, drawn with colour x_colors.line */

void dr_hline(struct Bitmap far *s, int x, int y, int w)
{
    char old;

    old = x_colors.fill;
    x_colors.fill = x_colors.line;
    dr_fill_rect(s, x, y, w, 1);
    x_colors.fill = old;
}

/* dr_bevel @ 0F38:4432 (linear 0x137B2, 421 bytes)

   Draw a raised box frame of width 3 around the rectangle (x, y, w, h) of bitmap s: four
   filled bars in colour x_colors.fill (x_colors.line when the style in x_colors.frame is 3), then an inner
   highlight made of two vertical and two horizontal lines, doubled when the style is 2 */

void dr_bevel(struct Bitmap far *s, int x, int y, int w, int h)
{
    int bm = x_colors.frame == 2 ? 3 : 3;
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
    old = x_colors.fill;
    if (x_colors.frame == 3)
        x_colors.fill = x_colors.line;
    dr_fill_rect(s, left, top8, bm, bh6);
    dr_fill_rect(s, x, bottom, w, bm);
    dr_fill_rect(s, right2, top8, bm, bh6);
    dr_fill_rect(s, x, top8, w, bm);
    if (x_colors.frame == 3)
        x_colors.fill = old;
    else {
        dr_vline(s, left + 1, top8 + 1, bh6 - 2);
        dr_hline(s, left + 1, bottom + 1, bw - 2);
        dr_vline(s, right2 + 1, top8 + 1, bh6 - 2);
        dr_hline(s, left + 1, top8 + 1, bw - 2);
        if (x_colors.frame == 2) {
            dr_vline(s, left + 2, top8 + 2, bh6 - 4);
            dr_hline(s, left + 2, bottom, bw - 4);
            dr_vline(s, right2, top8 + 2, bh6 - 4);
            dr_hline(s, left + 2, top8 + 2, bw - 4);
        }
    }
}

/* dr_frame @ 0F38:45D7 (linear 0x13957, 134 bytes)

   Draw a frame around the rectangle (x, y, w, h) of bitmap s, one pixel outside it:
   two vertical lines and two horizontal ones */

void dr_frame(struct Bitmap far *s, int x, int y, int w, int h)
{
    int x1;
    int x2;
    int y1;
    int y2;

    x1 = x - 1;
    x2 = x + w;
    y1 = y - 1;
    y2 = y + h;
    dr_vline(s, x1, y1, h + 2);
    dr_hline(s, x, y2, w);
    dr_vline(s, x2, y1, h + 2);
    dr_hline(s, x, y1, w);
}
