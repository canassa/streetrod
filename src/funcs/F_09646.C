/* F_09646 @ 0000:9646 (linear 0x09646, 520 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(abs)

static int near F_0E218(int n);

static void near F_09646(int flag)
{
    int result;
    struct Spec *item;
    struct Cand map[21];
    int m, found, k, low;

    for (k = 0; k < 21; k++) {
        item = &g_7d86[g_8000[k].f0];
        map[k].dr = abs(F_0E218(g_8000[k].f0) - F_0E218(-1));
        map[k].dk = abs(g_7d88[g_8000[k].f0].kind - g_7d88[g_7eb0->f2].kind);
        map[k].taken = (g_8000[k].f3 != -1) ? 0 : 1;
    }
    g_6c86 = 0;
    found = 1;
    do {
        if ((flag ? 21 : 9) <= g_6c86)
            break;
        found = 0;
        low = 0x7d00;
        for (m = 0; m < 21; m++)
            if (map[m].taken == 0 && map[m].dr < low) {
                result = m;
                low = map[m].dr;
                found = 1;
            }
        if (found) {
            g_6c8a[g_6c86++] = result;
            map[result].taken = 1;
        }
    } while (found);
    m = 0;
    k = g_6c86;
    for (; flag == 0 && k > 6; k--)
        if (map[k - 1].dk > 1)
            g_6c8a[k - 1] = g_6c8a[m++];
    if (g_0610) {
        g_6c8a[g_6c86++] = g_4ff4;
        g_6c8a[g_6c86++] = g_4ff4;
        if (g_6ca2) {
            g_6c8a[(g_6c86 = 1) - 1] = g_4ff4;
        }
        g_6ca2 = 0;
    }
}
