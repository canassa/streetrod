/* F_2834C @ 2645:1EFC (linear 0x2834C, 367 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(abs)

void F_2834C(void)
{
    int bits7, goal6, diff;

    g_7d30 += g_7d32 >> 3;
    F_27B6D(g_7d30);
    g_7d32 &= 7;
    bits7 = *g_7d62 & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g_7d82 = 2000;
            g_8bd0 = 0xaf0;
        } else if (bits7 & 0x20) {
            g_7d82 = 2000;
            g_8bd0 = 0xce4;
        } else {
            g_7d82 = 1500;
            g_8bd0 = 0xaf0;
        }
    } else {
        g_7d82 = 1500;
        g_8bd0 = 0xce4;
    }
    bits7 = g_7d62[4] & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g_70c0 = 2000;
            g_7d2e = 0xaf0;
        } else if (bits7 & 0x20) {
            g_70c0 = 2000;
            g_7d2e = 0xce4;
        } else {
            g_70c0 = 1500;
            g_7d2e = 0xaf0;
        }
    } else {
        g_70c0 = 1500;
        g_7d2e = 0xce4;
    }
    g_7d3e = *g_7880;
    if (g_0610 != 0 || g_0612 != 0 || (g_7ec0 == 0 ? 0x17c : 0x3fc) < g_7d30) {
        goal6 = 0x960;
        if (g_7e90 > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = g_7d36 - goal6;
        if (abs(diff) > 100)
            g_7d36 = goal6 + (goal6 < g_7d36 ? 100 : -100);
        else
            g_7d36 = goal6;
        if (abs(*g_7560 - g_7d3c) > 0) {
            g_78e0.f4 = (g_7d3c - *g_7560) * 2;
            if (abs(g_78e0.f4) > 24)
                g_78e0.f4 = g_78e0.f4 > 0 ? 24 : -24;
        }
        g_7d3c = *g_7560;
    }
}
