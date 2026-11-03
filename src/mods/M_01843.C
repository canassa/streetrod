#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#include <dos.h>

/* F_01843 @ 0000:1843 (linear 0x01843, 327 bytes) */


int F_01843(int delay, int pri)
{
    register int i, cur3;
    int prev;
    long when, key;

    g_0571 = 1;
    i = g_056a;
    if (i < 0) {
        g_0571 = 0;
        return -1;
    }
    cur3 = g_0568;
    prev = -1;
    when = g_tick + delay;
    if (delay) {
        for (;;) {
            if (cur3 >= 0) {
                if ((key = g_0572[cur3].key) > when) {
                    cur3 = g_0572[prev = cur3].nxt;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_0572[cur3].f8 >= pri)
                        break;
                    cur3 = g_0572[prev = cur3].nxt;
                    if (cur3 < 0 || g_0572[cur3].key != key)
                        break;
                }
            }
            break;
        }
    }
    g_0572[i].key = when;
    g_0572[i].f8 = pri;
    g_056a = g_0572[i].nxt;
    g_0572[i].nxt = cur3;
    if (cur3 < 0) {
        g_0566 = i;
        g_056c = when;
    } else
        g_0572[cur3].prv = i;
    g_0572[i].prv = prev;
    if (prev < 0)
        g_0568 = i;
    else
        g_0572[prev].nxt = i;
    g_0571 = 0;
    return i;
}

/* F_0198A @ 0000:198A (linear 0x0198A, 50 bytes) */


void F_0198A(int fc, int delay)
{
    int n;

    n = F_01843(delay, g_8ef6);
    g_0572[n].fa = 1;
    g_0572[n].fc = fc;
}

/* F_019BC @ 0000:19BC (linear 0x019BC, 73 bytes) */


void F_019BC(int n)
{
    struct Link *p;
    int i;

    for (i = g_0566; i >= 0; i = g_0572[i].nxt)
        if ((p = g_0572 + i)->fa == 1 && p->fc == n)
            p->fa = 0;
}

/* F_01A05 @ 0000:1A05 (linear 0x01A05, 119 bytes) */


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

/* F_01A7C @ 0000:1A7C (linear 0x01A7C, 49 bytes) */


#pragma intrinsic(inp, outp)

void F_01A7C(void)
{
    g_58d2->off = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    g_58d2->dirty = 1;
    g_58d2->lc = g_58d2->cur = g_58d2->l8;
}

/* F_01AAD @ 0000:1AAD (linear 0x01AAD, 122 bytes) */


#pragma intrinsic(inp, outp)

void F_01AAD(void)
{
    g_060a = 0;
    _dos_setvect(8, F_0238C);
    if (!g_0610 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 6;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
    g_78e0.f2 = g_78e0.f4 = 0;
    g_0609 = 0;
    outp(0x42, 0x1e);
    outp(0x42, 0);
    if (g_47ce) {
        outp(0x61, inp(0x61) | 3);
        g_47cf = 1;
    }
}

/* F_01B27 @ 0000:1B27 (linear 0x01B27, 106 bytes) */


#pragma intrinsic(inp, outp)

void F_01B27(void)
{
    _dos_setvect(8, F_01FE2);
    g_58dc = 0;
    g_58da = 0;
    g_5e0e = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    if (!g_0610 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 1;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
}

/* F_01B91 @ 0000:1B91 (linear 0x01B91, 29 bytes) */


int F_01B91(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}
