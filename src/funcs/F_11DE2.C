/* F_11DE2: pop up message window n with its position temporarily set to (x, y); idx negative coordinate keeps the default */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_11DE2(int n, int x, int y)
{
    int idx;
    int ox;
    int oy;

    idx = g_2c01[abs(n)] - 1;
    ox = g_04c4[idx].x;
    oy = g_04c4[idx].y;
    if (x >= 0)
        g_04c4[idx].x = x;
    if (y >= 0)
        g_04c4[idx].y = y;
    F_11C24(n);
    g_04c4[idx].x = ox;
    g_04c4[idx].y = oy;
    g_05de = 0;
}
