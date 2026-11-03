/* F_01A05 @ 0000:1A05 (linear 0x01A05, 119 bytes) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_01A05(int n, int cnt)
{
    if (g_58d0) {
        g_58d2->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
        g_58d2->f4 = 0;
        g_58d2->dirty = 1;
        g_58d2->lc = g_58d2->cur = g_58d2->l8 + n;
        g_58d2->f2 = cnt + 1;
        g_58d2->f1a = g_58d2->f18;
        g_82c4 = g_tones + *g_58d2->f1a;
        g_72ec = 1;
        g_58d2->off = 1;
    } else {
        g_58d2->off = 0;
    }
}
