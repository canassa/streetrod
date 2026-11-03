/* F_11C24: pop up message window n (negative n: the window is removed again afterwards).
   The text is the run of NUL-terminated lines that follows the window's index byte in g_2c01. */

#include <stdlib.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_11C24(int n)
{
    char *txt;
    int close;
    int x0;
    int y1;
    int w1;
    int h1;
    int rec;
    int lines;

    F_120FE(-3);
    if (g_04fa) {
        F_11B6F(g_04fa, g_6c40->x, g_6c40->y);
        F_19438(1);
        g_04fa = 0;
    }
    close = n < 0;
    if (close)
        n = -n;
    txt = g_2c01 + n + 1;
    rec = txt[-1] - 1;
    g_6c40 = &g_04c4[rec];
    g_04fa = F_11A72(x0 = g_6c40->x, y1 = g_6c40->y, w1 = g_6c40->w, h1 = g_6c40->h, 0);
    F_1A734(g_vid.back, g_6c40->bg, (unsigned char)x0, (unsigned char)y1, 0);
    g_colors = g_style->popup;
    x0 += g_6c40->tx;
    y1 += g_6c40->ty;
    lines = g_6c40->lines;
    while (lines--) {
        (*g_78ae)(g_vid.back, ((w1 - F_10B48(txt)) >> 1) + x0, y1, txt);
        txt = txt + strlen(txt) + 1;
        y1 += 9;
    }
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, g_6c40->x, g_6c40->y, g_6c40->x + w1 - 1, g_6c40->y + h1 - 1);
    if (close) {
        F_19438(1);
        g_04fa = 0;
    }
    g_05de = 1;
}
