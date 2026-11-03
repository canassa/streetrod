/* F_12CD1: for the marker items 0x43f/0x440 of the current slot, save a copy of the picture
   (shifted to the pixel grid) in g_0792 and set up the blit g_70ac that draws it. */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_12CD1(void)
{
    int what;
    char kind;
    int px;
    int i;
    int py;
    struct Bitmap far *picture;

    if (g_8290[1] == 0x437) {
        kind = g_077e[g_82ae];
        if (kind == 3)
            return;
        what = g_829c[i = (kind != 2) ? 1 : 0];
        if (what != 0x43f && what != 0x440)
            return;
        px = g_0732[i * 2] - 2;
        py = g_0732[i * 2 + 1];
        picture = F_15BBE(what, 1);
        g_0792 = F_1934E(picture->width + 8, picture->height, picture->alt.p != 0, 2);
        F_1A03F(picture, px & ~g_vid.mask, g_0792);
        F_1941E(1);
        g_70ac.w = g_0792->width;
        g_70ac.h = g_0792->height;
        g_70ac.sx = g_70ac.sy = 0;
        g_70ac.dx = (px & g_vid.mask) + 0x60;
        g_70ac.dy = py - g_70ac.h + 9;
    }
}
