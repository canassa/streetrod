#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_01FE2 @ 0000:1fe2 (linear 0x01fe2, 914 bytes) */


#pragma intrinsic(inp, outp, _enable, _disable)

void interrupt far F_01FE2(void)
{
    int x, y;
    register int delta;

    g_tick++;
    if (g_0608--) {
        outp(0x20, 0x20);
        _enable();
    } else {
        (*w_7aaa)();
        g_0608 = 3;
        g_0600++;
    }
    if (!g_47ce && g_47ce != g_47cf) {
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
    }
    if (g_0610) {
        if (((g_06a6 ? 0xf : 0x7f) & g_tick) == 0)
            F_01BAE();
        if ((g_06ae || g_06b0) && (g_472a != g_06ae || g_472c != g_06b0)) {
            g_472a += F_01B91(g_06ae - g_472a);
            g_472c += F_01B91(g_06b0 - g_472c);
            g_6c4a = 1;
        }
    }
    if (g_58d2->off && --g_58d2->f2) {
        if (t_72ec && g_58d2->f0.w && !--g_06e2) {
            bn82c4->f0 = bn82c4->f0 % bn82c4->f3;
            g_6c54 = g_57f6[bn82c4->f1][bn82c4->f0++] * g_6c52 + g_58d2->f0.w;
            outp(0x42, g_6c54);
            outp(0x42, g_6c54 >> 8);
            g_06e2 = bn82c4->f4;
        }
    } else if (g_58d2->off && g_58d2->f2 == 0) {
        while (!(g_58d2->f2 = *g_58d2->cur++)) {
            if (*g_58d2->cur++ == 0) {
                outp(0x61, inp(0x61) & 0xfc);
                g_47cf = 0;
                g_58d2->off = 0;
                g_58d2->dirty = 1;
                g_58d2->lc = g_58d2->cur = g_58d2->l8;
                goto tail;
            }
            g_58d2->f1a++;
            if (*g_58d2->f16 < ++g_58d2->f4) {
                if (*++g_58d2->f16 < 0) {
                    g_58d2->cur = g_58d2->l8 + 2;
                    g_58d2->f16 = g_58d2->f14;
                    g_58d2->f1a = g_58d2->f18;
                }
                g_58d2->lc = g_58d2->cur;
                g_58d2->f4 = 0;
            } else
                g_58d2->cur = g_58d2->lc;
            bn82c4 = g_tones + *g_58d2->f1a;
        }
        if (!(g_58d2->f0.w = *g_58d2->cur++)) {
            g_58d2->dirty = 1;
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        } else {
            if (t_72ec) {
                g_06e2 = bn82c4->f4;
                g_6c52 = (unsigned)bn82c4->f2 * g_58d2->f0.w / 1000;
            }
            outp(0x42, g_58d2->f0.b[0]);
            outp(0x42, g_58d2->f0.b[1]);
            if (g_47ce && (g_47ce != g_47cf || g_58d2->dirty)) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
                if (g_58d2->dirty)
                    g_58d2->dirty = 0;
            }
        }
    }
tail:
    if (g_4733 > 0) {
        _disable();
        if (!g_06e0) {
            g_06e0 = 1;
            _enable();
            x = y = 0;
            if (g_4740 == 0) {
                if (delta = g_472e)
                    x = delta > 0 ? 1 : -1;
                if (delta = g_472f)
                    y = delta > 0 ? 1 : -1;
            }
            if (g_0610 == 0 && g_471d) {
                F_02DF4();
                if ((delta = g_4718) < g_4722)
                    x = -1;
                else if (g_4724 < delta)
                    x = 1;
                if ((delta = g_471a) < g_4726)
                    y = -1;
                else if (g_4728 < delta)
                    y = 1;
            }
            if (x | y) {
                g_472a += g_474c[x];
                g_472c += g_4752[y];
                g_6c4a = 1;
            }
            g_06e0 = 0;
        }
    }
}
