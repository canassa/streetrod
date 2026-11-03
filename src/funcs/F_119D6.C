/* F_119D6: draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_119D6(int x, int y, char *s)
{
    (*g_78ae)(g_vid.back, x, y, s);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, g_824c - 1, y + 7);
}
