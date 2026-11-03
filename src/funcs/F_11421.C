#include "GLOBALS.H"
#include "PROTOS.H"

void F_11421(int x, char *s, int w)
{
    struct Colors saved;
    int left;
    int wd;
    int ypos;

    saved = g_colors;
    g_colors = g_style->colors;
    if (w == 0)
        w = g_8254;
    if (x < 0) {
        x = g_8250;
        left = (x + (w - F_10B48(s)) / 2) & 0xfff8;
    } else
        left = x;
    wd = 320 - x;
    ypos = g_8252;
    F_120FE(-3);
    if (g_8256)
        F_13682(g_vid.back, x - 1, ypos, wd, 8);
    (*g_78ae)(g_vid.back, left, ypos, s);
    g_8256 = *s;
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x > 8 ? x - 8 : x, ypos, x + wd, ypos + 7);
    g_colors = saved;
}
