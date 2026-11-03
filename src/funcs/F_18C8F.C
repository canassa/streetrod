/* F_18C8F: draw a bitmap into the work area, then copy it on to the front buffer */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_18C8F(struct Bitmap far **pbm)
{
    int w, x, h;

    F_15B7F(0x474, &w, &h, &x);
    F_03904(*pbm, g_731e->bits.p + 0x46 * g_731e->pitch, 0L, w, h, g_vid.mode == -2 ? -2 : -1);
    if (g_vid.mode == -2) {
        F_24E78(g_vid.front->bits.p, (*pbm)->bits.p - 0xaa0, (*pbm)->pitch, (*pbm)->height, 0, 0x44, 0x28);
    } else {
        F_11A72(0, 0x44, w, h, *pbm);
    }
}
