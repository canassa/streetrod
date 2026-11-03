/* F_0B425 @ 0000:B425 (linear 0x0B425, 49 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B425(void)
{
    int base, n;

    base = ((unsigned)g_19ea - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_1a44 - (unsigned)g_19ea) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}
