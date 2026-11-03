/* F_11BD3: draw bitmap bm at (x, y) on the back buffer and copy the covered rectangle to the front buffer */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_11BD3(struct Bitmap far *bm, int x, int y)
{
    F_11B6F(bm, x, y);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, bm->width + x - 1, bm->height + y - 1);
}
