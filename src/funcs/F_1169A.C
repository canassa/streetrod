#include "GLOBALS.H"
#include "PROTOS.H"

void F_1169A(int x, int y, int w, int h, char color)
{
    char old;

    old = g_8244;
    F_120FE(-3);
    g_8244 = color;
    F_13682(g_vid.back, x, y, w, h);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x, y, x + w - 1, y + h - 1);
    g_8244 = old;
    F_120FE(-1);
}
