/* F_137B2: draw a raised box frame of width 3 around the rectangle (x, y, w, h) of bitmap s: four
   filled bars in colour g_8244 (g_colors.c[3] when the style in g_colors.c[4] is 3), then an inner
   highlight made of two vertical and two horizontal lines, doubled when the style is 2 */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_137B2(struct Bitmap far *s, int x, int y, int w, int h)
{
    int bm = g_colors.c[4] == 2 ? 3 : 3;
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
    old = g_8244;
    if (g_colors.c[4] == 3)
        g_8244 = g_colors.c[3];
    F_13682(s, left, top8, bm, bh6);
    F_13682(s, x, bottom, w, bm);
    F_13682(s, right2, top8, bm, bh6);
    F_13682(s, x, top8, w, bm);
    if (g_colors.c[4] == 3)
        g_8244 = old;
    else {
        F_1371C(s, left + 1, top8 + 1, bh6 - 2);
        F_1377F(s, left + 1, bottom + 1, bw - 2);
        F_1371C(s, right2 + 1, top8 + 1, bh6 - 2);
        F_1377F(s, left + 1, top8 + 1, bw - 2);
        if (g_colors.c[4] == 2) {
            F_1371C(s, left + 2, top8 + 2, bh6 - 4);
            F_1377F(s, left + 2, bottom, bw - 4);
            F_1371C(s, right2, top8 + 2, bh6 - 4);
            F_1377F(s, left + 2, top8 + 2, bw - 4);
        }
    }
}
