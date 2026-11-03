#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

/* F_1628A: start the sound song with its tone list tones. Song g_79aa's own song restarts the
   first voice (g_58d4 = 1); any other song goes into the second voice g_79c6 (g_58d4 = 2), the
   first voice's on/off state g_58d0 is saved in g_58d6 meanwhile */


#pragma intrinsic(inp, outp)

int g_57f6[3][8] = {{0, 1, 2, 1, 0, -1, -2, -1}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 0, 0, 0, 0, 0, 0}};   /* DS:57F6 */
struct Tone g_tones[5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 8, 10}, {0, 0, 4, 8, 1}, {0, 0, 6, 8, 1}, {0, 2, 5, 2, 1}};   /* DS:5826 */

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

/* F_1633D @ 0F38:6FBD: if on, fill the first entry of the sound table; g_72ec = on either way */


void F_1633D(int on, int a, int b, int c, int d)
{
    if (on) {
        g_tones[0].f0 = 0;
        g_tones[0].f1 = a;
        g_tones[0].f2 = c;
        g_tones[0].f3 = b;
        g_tones[0].f4 = d;
    }
    g_72ec = on;
}

#pragma intrinsic(inp, outp)

void F_1636C(void)
{
    g_72ec = 0;
    outp(0x61, inp(0x61) & 0xfc);
    F_01A7C();
}

/* F_16381 @ 0F38:7001: toggle between two states (calls F_00D99 on entering, F_00DD8 on leaving) */


void F_16381(int on)
{
    if (on) {
        if (g_58d0) {
            F_00D99();
            g_58d0 = 0;
            g_58d8 = 1;
        } else
            g_58d8 = 0;
    } else {
        if (g_58d8) {
            g_58d0 = 1;
            F_00DD8();
            g_58d8 = 0;
        }
    }
}
