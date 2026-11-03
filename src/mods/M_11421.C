#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

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

void F_11540(int n)
{
    int old;
    char buf[10];

    old = g_8256;
    g_8256 = 1;
    F_11421(g_8258, ltoa(g_7eaa += n, buf, 10), 0x30);
    g_82c0 = g_824c;
    g_8256 = old;
}

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

void F_11669(int i)
{
    int saved;

    saved = g_8256;
    g_8256 = 1;
    F_11421(g_8258, g_msgs[i], 8);
    g_8256 = saved;
}
