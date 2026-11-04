#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Dev bs79aa;
struct Dev ao79c6t;
void (interrupt far *w_7aaa)(void);
char g7f32s[80];
int g8b8eb;
struct Lims ao8b90v;
struct Hot *at8ba8v;
int az8baa;
int cz8bc0y;
int g8ef6q;
int g_8ef8;
int aj8efax;
int bw8efc;

int g_0566 = -1;   /* DS:0566 */
int g_0568 = -1;   /* DS:0568 */
int g_056a = 0;   /* DS:056A */
long g_056c = 2147483647L;   /* DS:056C */
char g_0570 = 0;   /* DS:0570 */
char g_0571 = 0;   /* DS:0571 */
struct Link g_0572[4] = {{0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}};   /* DS:0572 */
int g_05ba = 0;   /* DS:05BA */
int g_05bc[8] = {0, 0, 0, 0, 0, 0, 0, 0};   /* DS:05BC */
long g_05cc = 0L;   /* DS:05CC */
int g_05d0 = 0;   /* DS:05D0 */
int g_05d2 = 0;   /* DS:05D2 */
int g_05d4 = 32767;   /* DS:05D4 */
int g_5d6 = 0;   /* DS:05D6 */
int g_05d8 = 0;   /* DS:05D8 */
int g_05da = 0;   /* DS:05DA */
int g_05dc = 350;   /* DS:05DC */
int g_05de = 0;   /* DS:05DE */
int g_05e0 = 0;   /* DS:05E0 */
int g_5e2 = 0;   /* DS:05E2 */
int g_5e4 = 0;   /* DS:05E4 */
int g_5e6 = 0;   /* DS:05E6 */
int g_5e8 = 0;   /* DS:05E8 */
int g_5ea = 0;   /* DS:05EA */
int g_5ec = 0;   /* DS:05EC */
int g_5ee = 0;   /* DS:05EE */
int g_5f0 = 10;   /* DS:05F0 */
char g_05f2 = 3;   /* DS:05F2 */
char g_05f3 = 0;   /* DS:05F3 */
int g_05f4[2] = {0, 0};   /* DS:05F4 */
long g_tick = 0L;   /* DS:05F8 */
long g_05fc = 0L;   /* DS:05FC */
long g_0600 = 0L;   /* DS:0600 */
long g_0604 = 0L;   /* DS:0604 */
char g_0608 = 0;   /* DS:0608 */
char g_0609 = 0;   /* DS:0609 */
int g_060a = 0;   /* DS:060A */
char *g_060c = "cannot read drive ?";   /* DS:060C */
char *g_060e = "disk ? is write protected";   /* DS:060E */
int g_0610 = 0;   /* DS:0610 */
int g_0612 = 0;   /* DS:0612 */

/* F_00D78 @ 0000:0D78 (linear 0x00D78, 33 bytes) */


void F_00D78(void)
{
    g_6c4a = 1;
    ao8b90v.x2 = -1;
    az8baa = 0x7d00;
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
    _dos_setvect(8, w_7aaa);
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
        (*m_present)();
        F_0E610();
    }
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    exit(5);
}

char g_064d = 0;   /* DS:064D */
char g_064e = 0;   /* DS:064E */
struct KeyMap g_0650[30] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};   /* DS:0650 */

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
    g8ef6q = 10;
    g_8ef8 = 2;
    aj8efax = 5;
    bw8efc = 5;
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
    bs79aa.off = 0;
    bs79aa.l8 = g_2e6b_0000;
    bs79aa.cur = bs79aa.lc = bs79aa.l8;
    bs79aa.f16 = bs79aa.f14 = g_5858;
    bs79aa.f1a = bs79aa.f18 = g_5882;
    ao79c6t.off = 0;
    ao79c6t.f1a = ao79c6t.f18 = g_58cc;
    _harderr(F_0105B);
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0x40);
    w_7aaa = _dos_getvect(8);
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
    p = g_579f[42];
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

unsigned char g_0692[16] = {0, 0, 8, 7, 15, 7, 15, 8, 7, 2, 4, 12, 5, 10, 6, 14};   /* DS:0692 */
int g_06a2 = 0;   /* DS:06A2 */
int g_06a4 = 0;   /* DS:06A4 */
char g_06a6 = 0;   /* DS:06A6 */
char g_06a7 = 0;   /* DS:06A7 */
int g_06a8 = 0;   /* DS:06A8 */
int g_06aa = 0;   /* DS:06AA */
int g_06ac = 0;   /* DS:06AC */
int g_06ae = 0;   /* DS:06AE */
int g_06b0 = 0;   /* DS:06B0 */
int g_06b2 = 0;   /* DS:06B2 */
int g_06b4[22] = {-1, 0, -1, 0, 1, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 0, 0, 2};   /* DS:06B4 */
char g_06e0 = 0;   /* DS:06E0 */
int g_06e2 = 1;   /* DS:06E2 */
int g_06e4 = 0;   /* DS:06E4 */
int g_06e6 = 0;   /* DS:06E6 */
char g_06e8 = 0;   /* DS:06E8 */
char g_06e9 = 0;   /* DS:06E9 */
char g_06ea = 0;   /* DS:06EA */

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
        aq8256z = 1;
        F_11421(-1, "", 0);
        aq8256z = 1;
        F_11421(g_51ee, "", cz8bc0y);
        g7f32s[0] = 0;
    }
    flag = g_05cc != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_0610 == 0x63 && g_06a4 != 0x63) {
            g_06a4 = 0x63;
            F_00FEA();
        }
        if (g_4bc8 && (r_vid.mode == -2 || r_vid.mode == -6)
            && !(g_tick & 0xf) && g_58d0 && g_47cf) {
            old = ((char *)((char *)g_0692 + 6))[2];
            ((char *)((char *)g_0692 + 6))[2] = ((char *)((char *)g_0692 + 6))[1];
            ((char *)((char *)g_0692 + 6))[1] = ((char *)((char *)g_0692 + 6))[0];
            ((char *)((char *)g_0692 + 6))[0] = old;
            F_112CB(6, old);
            F_112CB(7, ((char *)((char *)g_0692 + 6))[1]);
            F_112CB(8, ((char *)((char *)g_0692 + 6))[2]);
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
            if (g8b8eb) {
                if (i = (x >= ao8b90v.x1 && x < ao8b90v.x2 && y >= ao8b90v.y1 && y < ao8b90v.y2)
                        ? az8baa : F_1412D(x, y))
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
            if (g8b8eb
                && (x < ao8b90v.x1 || x >= ao8b90v.x2 || y < ao8b90v.y1 || y >= ao8b90v.y2)) {
                res = az8baa;
                if (i = F_1412D(x, y)) {
                    if (res != i) {
                        if (g_6c4c == 0x25) {
                            aq8256z = 1;
                            F_11421(-1, "", 0);
                            aq8256z = 1;
                            F_11421(g_51ee, "", cz8bc0y);
                        }
                        F_11421(-1, g_239e + at8ba8v->f4, 0);
                        bit = 1;
                    }
                } else {
                    if (bit) {
                        F_11421(-1, "", 0);
                        bit = 0;
                    }
                    if (g_6c4c == 0x25)
                        F_11421(g_51ee, g7f32s, cz8bc0y);
                    else if (res)
                        F_11421(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}
