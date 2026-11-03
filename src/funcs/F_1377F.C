/* F_1377F: horizontal line of width w at (x, y) in bitmap s, drawn with colour g_colors.c[3] */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1377F(struct Bitmap far *s, int x, int y, int w)
{
    char old;

    old = g_8244;
    g_8244 = g_colors.c[3];
    F_13682(s, x, y, w, 1);
    g_8244 = old;
}
