/* F_1628A: start the sound song with its tone list tones. Song g_79aa's own song restarts the
   first voice (g_58d4 = 1); any other song goes into the second voice g_79c6 (g_58d4 = 2), the
   first voice's on/off state g_58d0 is saved in g_58d6 meanwhile */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_1628A(void far *song, void *tones)
{
    if (song == g_79aa.l8) {
        if (g_58d6 != -1) {
            g_58d0 = g_58d6;
            g_58d6 = -1;
        }
        if (g_58d0 == 0) {
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        }
        g_58d4 = 1;
        g_58d2 = &g_79aa;
        g_82c4 = g_tones + *g_79aa.f1a;
        g_72ec = 1;
    } else {
        if (g_58d6 == -1) {
            g_58d6 = g_58d0;
            g_58d0 = 1;
        }
        g_58d4 = 2;
        g_79c6.cur = g_79c6.lc = g_79c6.l8 = song;
        g_79c6.f16 = g_79c6.f14 = tones;
        g_58d2 = &g_79c6;
        F_01A05(2, 0);
    }
}
