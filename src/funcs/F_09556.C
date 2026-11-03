/* F_09556 @ 0000:9556 (linear 0x09556, 155 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_09556(void)
{
    int k, any, total;

    any = 0;
    for (k = 0; k < 21; k++)
        if (g_8000[k].f3 != -1)
            any = 1;
    if (any == 0)
        return 0;
    total = g_4972 + g_4974;
    if (total < 3)
        return 1;
    if (total >= 7 && g_4972 < 6 && g_4974 < 6)
        return 3;
    if (total < 7 && g_4972 < 6 && g_4974 < 6)
        return 2;
    if (g_4972 < 6)
        return 4;
    if (g_4974 < 6)
        return 5;
    if (total >= (g_4970 >> 1))
        return 0;
    return 3;
}
