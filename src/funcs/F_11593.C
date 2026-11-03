#include "GLOBALS.H"
#include "PROTOS.H"

void F_11593(char *s)
{
    int saved;

    saved = g_vid.f16;
    g_colors = g_style->colors;
    F_120FE(-3);
    F_13682(g_vid.back, 0, g_8252, g_8250 & 0xfff8, 8);
    (*g_78ae)(g_vid.back, 0, g_8252, s);
    g_8258 = (g_824c + 7) & 0xfff8;
    g_vid.f16 = 0;
    if (s[1] != 'G' && s[1] != ' ')
        F_11540(0);
    g_vid.f16 = saved;
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, 0, g_8252, g_8250, g_8252 + 7);
}
