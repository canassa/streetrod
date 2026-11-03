/* F_06827 @ 0000:6827 (linear 0x06827, 91 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_06827(int id)
{
    int n, k, base;

    base = ((unsigned)g_121c - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    for (k = 0; k < n; k++)
        if (g_09f4[base + k].f6 == id)
            return base + k;
}
