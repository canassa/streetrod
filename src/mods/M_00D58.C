#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"

/* F_00D58 @ 0000:0D58 (linear 0x00D58, 32 bytes) */


void F_00D58(void)
{
    F_193D4();
    g_pages.list = 0;
    g_051c = 0;
    F_13F21(1);
}

/* F_00D78 @ 0000:0D78 (linear 0x00D78, 33 bytes) */


void F_00D78(void)
{
    g_6c4a = 1;
    g_8b90.x2 = -1;
    g_8baa = 0x7d00;
    g_4736 = 0;
    g_4737 = 0;
    F_02DB6();
}

/* F_00D99 @ 0000:0D99 (linear 0x00D99, 63 bytes) */


#pragma intrinsic(inp, outp)

void F_00D99(void)
{
    if (g_58d2->off) {
        g_58d2->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2bab;
        F_00D78();
    }
}

/* F_00DD8 @ 0000:0DD8 (linear 0x00DD8, 76 bytes) */


void F_00DD8(void)
{
    if (g_58d0 && !g_58d2->off && g_58d2->lc != g_58d2->l8) {
        g_58d2->off = 1;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2ba9;
        F_00D78();
    }
}

/* F_00E24 @ 0000:0E24 (linear 0x00E24, 231 bytes) */


void F_00E24(void)
{
    int i;
    long deadline;

    g_0566 = g_0568 = -1;
    g_056a = 0;
    g_056c = 0x7fffffffL;
    g_0570 = g_0571 = 0;
    for (i = 0; i < 4; i++)
        g_0572[i].nxt = i + 1;
    g_05ba = -1;
    deadline = g_tick + 10;
    while (deadline > g_tick) {
        for (i = 0; F_02DB6(); i++)
            ;
        if (i)
            deadline = g_tick + 10;
    }
    g_05cc = 0;
    if (g_0610 == 0 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
    }
    g_4736 = 0;
    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_4732 = g_472e = g_472f = g_4730 = g_4731 = 0;
}

/* F_00F0B @ 0000:0F0B (linear 0x00F0B, 172 bytes) */


void F_00F0B(int n)
{
    register int i;

    i = g_0572[n].nxt;
    if (i >= 0)
        g_0572[i].prv = g_0572[n].prv;
    else {
        i = g_0572[n].prv;
        g_0566 = i;
        if (i < 0)
            g_056c = 0x7fffffffL;
        else
            g_056c = g_0572[i].key;
    }
    i = g_0572[n].prv;
    if (i >= 0)
        g_0572[i].nxt = g_0572[n].nxt;
    else
        g_0568 = g_0572[n].nxt;
    g_0572[n].nxt = g_056a;
    g_056a = n;
}

/* F_00FB7 @ 0000:0FB7 (linear 0x00FB7, 51 bytes) */


#pragma intrinsic(outp)

void F_00FB7(void)
{
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);
    _dos_setvect(8, g_7aaa);
    _enable();
    F_02DE2();
    g_05e0 = 0;
    g_0044 = 0;
}

/* F_00FEA @ 0000:0FEA (linear 0x00FEA, 113 bytes) */


#pragma intrinsic(inp, outp)

void F_00FEA(void)
{
    if (g_0610) {
        g_0610 = (g_0610 == 0x62) ? 0 : 1;
        F_1174E("Quit demo");
        F_1169A(0, 0, 320, 200, 0);
    }
    if (g_0044)
        F_00FB7();
    if (g_0048) {
        (*g_present)();
        F_0E610();
    }
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    exit(5);
}

/* F_0105B @ 0000:105B (linear 0x0105B, 182 bytes) */


void F_0105B(unsigned deverr, unsigned char errcode)
{
    int disk;
    int action;

    disk = (deverr & 0x8000) == 0;
    if (disk) {
        if (deverr & 0x1000)
            action = 1;
        else
            action = 2;
    } else
        action = 1;
    switch (errcode) {
    case 0:
        g_060e[5] = deverr + 'A';
        F_1174E(g_060e);
        break;
    case 2:
        g_060c[strlen(g_060c) - 1] = deverr + 'A';
        F_1174E(g_060c);
        break;
    case 9:
        F_1174E("printer out of paper");
        break;
    default:
        if (disk)
            F_1174E("disk error");
        else {
            F_1174E("hardware error");
            action = 2;
        }
    }
    if (action == 2)
        F_00FEA();
    _hardresume(action);
}

/* F_01111 @ 0000:1111 (linear 0x01111, 461 bytes) */


#pragma intrinsic(inp, outp)

void F_01111(void)
{
    int sumx;
    int sumy;
    int first;
    int v;
    int j;

    sumx = 0;
    sumy = 0;
    g_8ef6 = 10;
    g_8ef8 = 2;
    g_8efa = 5;
    g_8efc = 5;
    outp(0x201, 0);
    outp(0x201, 0);
    first = inp(0x201);
    for (j = 1; j; j++)
        if (((v = inp(0x201)) & 0xf3) == 0xf0)
            break;
    if (g_0610 == 0 && j != 0 && v != first) {
        g_471d = 1;
        F_02DF4();
        sumx = g_4718;
        sumy = g_471a;
        F_02DF4();
        sumx += g_4718;
        sumy += g_471a;
        F_02DF4();
        sumx += g_4718;
        sumy += g_471a;
        g_471e = sumx / 3;
        g_4720 = sumy / 3;
        g_4722 = -(g_471e >> 1);
        g_4724 = g_471e >> 1;
        g_4726 = -(g_4720 >> 1);
        g_4728 = g_4720 >> 1;
    }
    if (g_0610 == 0 && g_0262 && F_16D1A() == -1) {
        g_05f2 = 1;
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 1;
    } else
        g_05f2 = 0;
    g_6c46 = -1;
    g_6c48 = -1;
    g_79aa.off = 0;
    g_79aa.l8 = g_2e6b_0000;
    g_79aa.cur = g_79aa.lc = g_79aa.l8;
    g_79aa.f16 = g_79aa.f14 = g_5858;
    g_79aa.f1a = g_79aa.f18 = g_5882;
    g_79c6.off = 0;
    g_79c6.f1a = g_79c6.f18 = g_58cc;
    _harderr(F_0105B);
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0x40);
    g_7aaa = _dos_getvect(8);
    _dos_setvect(8, F_01FE2);
    _enable();
    F_02DC3();
    g_05e0 = 1;
    outp(0x43, 0xb6);
    F_00E24();
    g_0044 = 1;
}

/* F_012DE @ 0000:12DE (linear 0x012DE, 179 bytes) */


void F_012DE(int n)
{
    struct KeyMap *p;
    int num;
    struct KeyMap *q;
    char c;

    num = 1;
    if (g_0610)
        g_47cc = 0;
    g_6c4c = n;
    F_01391(0);
    g_064e = n;
    p = g_579f[n];
    q = g_0650;
    for (; p->key; p++, q++, num++) {
        if (num >= 30)
            break;
        q->val = g_4766[q->key = c = p->key];
        g_4766[c] = p->val;
    }
    p = g_57f3;
    q->val = g_4766[q->key = c = p->key];
    g_4766[c] = p->val;
    q[1].key = 0;
}

/* F_01391 @ 0000:1391 (linear 0x01391, 134 bytes) */


int F_01391(int keep)
{
    int i;
    struct KeyMap *p;

    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_472e = g_472f = g_4730 = g_4731 = 0;
    i = g_064e;
    if (i) {
        if (keep) {
            for (p = g_0650; p->key; p++)
                if (p->val >= 0x20)
                    g_4766[p->key] = p->val;
        } else {
            for (p = g_0650; p->key; p++)
                g_4766[p->key] = p->val;
            g_064e = 0;
        }
    }
    return i;
}

/* F_01417 @ 0000:1417 (linear 0x01417, 1068 bytes) */



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
