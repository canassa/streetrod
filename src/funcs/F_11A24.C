/* F_11A24: draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_11A24(int x, int y, int w)
{
    F_1377F(g_vid.back, x, y, w);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, x + w - 1, y);
}
