/* F_01417 @ 0000:1417 (linear 0x01417, 1068 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"


int F_01417(int ticks)
{
    register struct Link *p;
    long end;
    int x;
    register int i;
    int y;
    int res;
    char flag;
    int j;
    char old;
    int bit;

    res = 0;
    j = 0;
    bit = 0;
    if (g_6c4c == 0x16) {
        g_8256 = 1;
        F_11421(-1, "", 0);
        g_8256 = 1;
        F_11421(g_51ee, "", g_8bc0);
        g_7f32[0] = 0;
    }
    flag = g_05cc != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_0610 == 0x63 && g_06a4 != 0x63) {
            g_06a4 = 0x63;
            F_00FEA();
        }
        if (g_4bc8 && (g_vid.mode == -2 || g_vid.mode == -6)
            && !(g_tick & 0xf) && g_58d0 && g_47cf) {
            old = g_0698[2];
            g_0698[2] = g_0698[1];
            g_0698[1] = g_0698[0];
            g_0698[0] = old;
            F_112CB(6, old);
            F_112CB(7, g_0698[1]);
            F_112CB(8, g_0698[2]);
        }
        res = F_02DB6();
        if (res > 0) {
            if (g_05d0) {
                g_05d2 = res;
                return 0x3eb;
            }
            if (res == 0x7f)
                F_00FEA();
        } else if (res < 0) {
            res += 0x32;
            if (res == -0x28) {
                if (g_58d4 == 1) {
                    if (g_58d0) {
                        F_00D99();
                        g_58d0 = 0;
                    } else {
                        g_58d0 = 1;
                        F_00DD8();
                    }
                }
            } else
                return res;
        }
        if (g_056c <= g_tick) {
            p = &g_0572[i = g_0566];
            switch (p->fa) {
            case 0:
            default:
                F_00F0B(i);
                break;
            case 1:
                res = p->fc;
                F_00F0B(i);
                F_111C1(res);
                break;
            }
        }
        if (g_0610 == 0 && g_05f2) {
            F_16DC7();
            if (g_6c4a |= (g_5e2 != 0 || g_5e4 != 0)) {
                g_472a += g_5e2;
                g_472c += g_5e4;
            }
        }
        if (g_4736 | g_4737) {
            _disable();
            g_6c4e = g_4736;
            x = g_4738;
            y = g_473a;
            g_4737 = g_4736 = 0;
            _enable();
            if (g_05de) {
                g_05d2 = 0;
                return 0x3eb;
            }
            if (g_8b8e) {
                if (i = (x >= g_8b90.x1 && x < g_8b90.x2 && y >= g_8b90.y1 && y < g_8b90.y2)
                        ? g_8baa : F_1412D(x, y))
                    return i;
            }
            if (g_05d0) {
                g_05d2 = 0;
                return 0x3eb;
            }
        } else if (flag && g_05cc < g_tick) {
            g_05cc = 0L;
            return 0x3e8;
        }
        if (g_4733 > 0 && g_6c4a) {
            _disable();
            if ((g_6c46 = g_472a) < 0)
                g_6c46 = g_472a = 0;
            else if (g_6c46 > 0x13a)
                g_6c46 = g_472a = 0x13a;
            if ((g_6c48 = g_472c) < 0)
                g_6c48 = g_472c = 0;
            else if (g_6c48 > 0xc2)
                g_6c48 = g_472c = 0xc2;
            g_6c4a = 0;
            _enable();
            F_1220B(x = g_6c46, y = g_6c48);
            if (g_8b8e
                && (x < g_8b90.x1 || x >= g_8b90.x2 || y < g_8b90.y1 || y >= g_8b90.y2)) {
                res = g_8baa;
                if (i = F_1412D(x, y)) {
                    if (res != i) {
                        if (g_6c4c == 0x25) {
                            g_8256 = 1;
                            F_11421(-1, "", 0);
                            g_8256 = 1;
                            F_11421(g_51ee, "", g_8bc0);
                        }
                        F_11421(-1, g_239e + g_8ba8->f4, 0);
                        bit = 1;
                    }
                } else {
                    if (bit) {
                        F_11421(-1, "", 0);
                        bit = 0;
                    }
                    if (g_6c4c == 0x25)
                        F_11421(g_51ee, g_7f32, g_8bc0);
                    else if (res)
                        F_11421(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}
