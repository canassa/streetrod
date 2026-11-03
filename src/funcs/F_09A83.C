/* F_09A83 @ 0000:9A83 (linear 0x09A83, 113 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_09556(void);

static void near F_09A83(int *ids, int *slots, int *kinds)
{
    int i, n;

    if (F_09556() == 0)
        g_817d = 1;
    else
        g_817d = 0;
    for (i = 0, n = 0; i <= 21; i++) {
        if (g_7ff8[i].fb == 1) {
            slots[n] = i;
            ids[++n] = g_7ff8[i].f6;
            kinds[n] = 7;
        }
    }
    ids[0] = n;
}
