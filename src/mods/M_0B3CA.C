#include "GLOBALS.H"
#include "PROTOS.H"

int g_53b6[6] = {2, 1, 4, 6, 5, 3};   /* DS:53B6 */

/* F_0B3CA @ 0000:B3CA (linear 0x0B3CA, 91 bytes) */


static int near F_0B3CA(int id)
{
    int base, n, i;

    base = ((unsigned)g_19ea - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_1a44 - (unsigned)g_19ea) / sizeof(struct Hot) + 1;
    for (i = 0; i < n; i++)
        if (g_09f4[base + i].f6 == id)
            return base + i;
}

/* F_0B425 @ 0000:B425 (linear 0x0B425, 49 bytes) */


static void near F_0B425(void)
{
    int base, n;

    base = ((unsigned)g_19ea - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_1a44 - (unsigned)g_19ea) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}
