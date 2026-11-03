/* F_10414 @ 0F38:1094 (linear 0x10414, 156 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_10414(int n)
{
    int x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    seg = g_787a;
    off = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa200 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
    F_28840();
    x = g_031e[n][0];
    y2 = g_031e[n][1] + 100;
    ymax = g_clip_ymax;
    ymin2 = g_clip_ymin;
    g_clip_ymax = 190;
    g_clip_ymin = 100;
    F_29F0F(x, y2);
    g_clip_ymax = ymax;
    g_clip_ymin = ymin2;
    F_28860();
    g_787a = seg;
    g_767c = off;
}
