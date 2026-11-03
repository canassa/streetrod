#include "GLOBALS.H"
#include "PROTOS.H"

void F_113A2(int y)
{
    g_8252 = y + 1;
    g_8254 = 0xa0;
    g_8250 = 0x58;
    g_8256 = 0;
    g_colors = g_style->colors;
    F_13682(g_vid.back, 0, y, 320, 10);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, 0, y, 319, y + 9);
}
