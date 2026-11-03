/* F_12DD8: draw item picture n at the current slot position, aligned to the pixel grid;
   flag (nonzero) raises it by 20 and skips the blit to the back buffer. Frees the temporary copies. */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_12CD1(void);

void F_12DD8(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused;
    int w;
    struct Blt move;
    int x;
    int height;
    int z;
    int ypos;
    int mask;

    z = 0;
    F_12909(&x, &ypos);
    if (g_06fc)
        F_120FE(-4);
    saved = pict = F_15BBE(n, 2);
    w = saved->width;
    if (mask = x & ~g_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, mask, saved);
        w = saved->width;
    }
    x &= g_vid.mask;
    if (n == 0x43c)
        F_12CD1();
    if (n != 0x434 && n != 0x435 && n != 0x436)
        ypos -= saved->height;
    if (flag)
        ypos -= 0x14;
    move.w = w;
    height = move.h = saved->height;
    move.sx = move.sy = 0;
    if (flag == 0) {
        move.dx = x;
        move.dy = ypos;
        (*g_78aa)(saved, g_vid.f8, &move, 1);
    }
    if (g_06fc) {
        (*g_78ce)(0xd, 0x93, x + 0x60, g_vid.f16 ? 0xd - height : ypos, ypos + 9, saved);
        F_120FE(-2);
    }
    if (g_0792) {
        F_19438(1);
        g_0792 = 0;
    }
    F_19438(1);
    if (pict != saved)
        F_19438(1);
}
