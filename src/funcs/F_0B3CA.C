/* F_0B3CA @ 0000:B3CA (linear 0x0B3CA, 91 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0B3CA(int id)
{
    int base, n, i;

    base = ((unsigned)g_19ea - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_1a44 - (unsigned)g_19ea) / sizeof(struct Hot) + 1;
    for (i = 0; i < n; i++)
        if (g_09f4[base + i].f6 == id)
            return base + i;
}
