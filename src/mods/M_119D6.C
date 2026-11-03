#include "GLOBALS.H"
#include "PROTOS.H"

/* F_119D6: draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */


void F_119D6(int x, int y, char *s)
{
    (*g_78ae)(g_vid.back, x, y, s);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, g_824c - 1, y + 7);
}

/* F_11A24: draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */


void F_11A24(int x, int y, int w)
{
    F_1377F(g_vid.back, x, y, w);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, x + w - 1, y);
}
