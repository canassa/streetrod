/* F_06882 @ 0000:6882 (linear 0x06882, 49 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_06882(void)
{
    int base, n;

    base = ((unsigned)g_121c - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}
