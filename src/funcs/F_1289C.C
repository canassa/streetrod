/* F_1289C: position (x, y) of the current slot in the lower table; returns the offset for the item type */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1289C(int *px, int *py)
{
    int r;
    int t;

    switch (g_8290[g_077e[g_82ae]]) {
    case 0x43c:
        r = 0;
        break;
    case 0x43d:
        r = 2;
        break;
    case 0x43e:
        r = 4;
        break;
    }
    *px = g_0742[t = g_82ae - 4][0];
    *py = g_0742[t][1];
    return r;
}
