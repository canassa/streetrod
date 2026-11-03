/* F_0A0F8 @ 0000:A0F8 (linear 0x0A0F8, 365 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0A0F8(int a, int b)
{
    struct Hot save;
    struct Hot *hot;
    int y1, x1, y, x;

    hot = g_20aa;
    if (g_20aa[4].f6 != 4) {
        save = g_20aa[3];
        g_20aa[3] = g_20aa[4];
        g_20aa[4] = save;
    }
    hot[4].x1 = 0xc2;
    hot[4].x2 = 0xd8;
    hot[4].y1 = 0x49;
    hot[4].y2 = 0x77;
    hot[2].f0 = (b == 1) ? 11 : 14;
    if (a == 0) {
        hot[1].f0 = 14;
        hot[3].f0 = 14;
    } else {
        x1 = g_70e8[g_7648].x + g_825e;
        y1 = g_70e8[g_7648].y + g_8260;
        x = g_70e8[g_7648].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].f0 = 11;
        x1 = g_70e8[g_7648].sx + g_825e;
        y1 = g_70e8[g_7648].sy + g_8260;
        x = g_70e8[g_7648].sw + x1 - 1;
        y = g_70e8[g_7648].sh + y1 - 1;
        if (hot[1].x2 >= x1)
            x1 = hot[1].x2 + 1;
        hot[3].x1 = x1;
        hot[3].x2 = x;
        hot[3].y1 = y1;
        hot[3].y2 = y;
        hot[3].f0 = 11;
        if (hot[4].y2 >= hot[1].y1)
            hot[4].y2 = hot[1].y1 - 1;
        if (hot[4].x1 < hot[3].x1) {
            save = hot[3];
            hot[3] = hot[4];
            hot[4] = save;
        }
    }
}
