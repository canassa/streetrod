#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static int near F_0E218(int n);
static void near F_0CDAF(struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);
static void near F_0CF5C(void);
static void near F_0D1DE(int rpm, struct Eng *p);

/* F_0CDAF @ 0000:CDAF (linear 0x0CDAF, 429 bytes) */



static void near F_0CDAF(struct Eng *p)
{
    struct Node *car;
    int t;

    _disable();
    if (g_4746 == 0 && g_4748 == 0) {
        g_4746 = g_472a;
        g_4748 = g_472c;
    }
    g_472a = g_472c = g_4730 = g_4731 = 0;
    _enable();
    g_47cd = 0;
    p->f0 = p->f18 = p->f42 = p->f2 = p->f4 = 0;
    p->fbc = p->fbe = p->fc0 = 0;
    if (p->f20) {
        p->gear[2].r[1].f2 = p->gear[2].r[2].f0 = p->f1e;
        p->f24 = 1;
    }
    p->f28 = p->f26 = -1;
    p->fe = 0x5dc;
    p->f40 = p->f2a;
    p->f14 = p->gear[0].r[0].f4;
    p->f16 = p->gear[0].r[0].f6;
    p->f12 = p->gear[0].r[0].f2;
    p->f10 = p->gear[0].r[0].f0;
    g_5d6 = 0;
    g_05d8 = g_05da = 0;
    g_8630 = -1;
    g_6caa = g_7ec0 == 0 ? 0x17c : 0x3fc;
    car = g_7eb0->slot12;
    p->fc = g_54b2[((struct Row8 *)((char *)g_4806 + 2))[car->idx].kind][car->b / 2000];
    if (car->b < 8000)
        p->fa = 0;
    else {
        if (car->b + random_int(2000) <= 9500)
            p->fa = 0;
        else {
            t = (g_6caa - 200) >> 1;
            p->fa = random_int(t >> 1) + t + 200;
        }
        goto nx;
    }
nx:
    g_787c = 0;
    g_54d0 = 0;
    g_6cac = F_0E218(g_7648);
    g_6cb2 = (int)(g_6cac * 0.7);
    g_6cb0 = (int)(g_6cac * 0.8);
    g_6cae = g_8acc < 4 ? 10 : 16;
    g_54d2 = 0;
    g_53e6 = 0;
}

/* F_0CF5C @ 0000:CF5C (linear 0x0CF5C, 213 bytes) */



static void near F_0CF5C(void)
{
    struct Range *r;

    F_0E0E2(g_7648, &g_79e2);
    g_6cb4 = g_7d84->f1;
    F_0CDAF(&g_79e2);
    g_79e2.f18 = 0;
    g_79e2.f40 = g_79e2.f34[0];
    g_79e2.fe = 0x5dc;
    g_79e2.f0 = 0x5dc / g_79e2.f40;
    if ((r = g_79e2.gear[0].r + (g_6cb8 = 1))->f2 <= 0x5dc) {
        g_6cb8 = 2;
        r++;
    } else if (r->f0 > g_79e2.fe) {
        g_6cb8 = 0;
        r--;
    }
    g_79e2.f42 = g_6cb8;
    g_79e2.f12 = r->f2;
    g_79e2.f14 = r->f4;
    g_6cba = g_79e2.f24 ? 0x14b4 : g_79e2.f1e - 100 * g_6cb4;
    g_7e8a = 200;
    g_7e8c = 0;
    g_6cb6 = g_7e90 = ((g_70c0 + g_7d2e) >> 1) - 200;
}

/* F_0D031 @ 0000:D031 (linear 0x0D031, 429 bytes) */



static void near F_0D031(void)
{
    int one;
    struct Node *slot;

    one = 1;
    F_120FE(-4);
    F_120FE(1);
    F_00E24();
    F_0CDAF(&g_78e0);
    F_0CF5C();
    if (g_7ec0 == 1 && g_7d40 == 0) {
        if (g_7f88 = !random_int(4))
            g_755a = random_int(255) + 0x1fe;
    } else
        g_7f88 = 0;
    g_79a6 = 0;
    g_8aca = 1;
    F_08E2D();
    F_01B27();
    g_05cc = 0;
    F_00E24();
    F_11593(g_49dc);
    if (g_7d30 > 200) {
        if (random_int(3) > 0) {
            if (g_7eb0->f20 > 0)
                g_7eb0->f20 = g_7eb0->f20 < random_int(4) ? random_int(4) : g_7eb0->f20;
            else if (g_7eb0->f20 < 0)
                g_7eb0->f20 = -random_int(8) < g_7eb0->f20 ? -random_int(8) : g_7eb0->f20;
            else
                g_7eb0->f20 = random_int(8) - 4;
        }
        slot = g_7eb0->slot12;
        slot->b += (((struct Row8 *)((char *)g_4806 + 2))[slot->idx].kind == 0 ? (g_7ec0 == 1 ? 700 : 300) : (g_7ec0 == 1 ? 400 : 200)) + random_int(300);
        if (slot->b > 10000)
            slot->b = 10000;
    }
}

/* F_0D1DE @ 0000:D1DE (linear 0x0D1DE, 191 bytes)

   Steps p->f18 up from 1 until gear[f18].r[1].f2 exceeds f34[f18] * rpm or
   f18 reaches f1c. The four pointers the loop advances in memory are /Ol induction temps for the p->f18 indexing. */


static void near F_0D1DE(int rpm, struct Eng *p)
{
    struct Range *r;

    for (p->f18 = 1; ; p->f18++) {
        p->f40 = p->f34[p->f18];
        p->fe = p->f40 * rpm;
        if ((r = &p->gear[p->f18].r[1])->f2 > p->fe) {
            p->f42 = g_6cb8 = p->fe >= r->f0;
            if (g_6cb8 == 0)
                r = &p->gear[p->f18].r[0];
            break;
        }
        if (p->f18 == p->f1c) {
            r = &p->gear[p->f18].r[2];
            p->f42 = g_6cb8 = 2;
            break;
        }
    }
    p->f14 = r->f4;
    p->f0 = p->fe / p->f40;
}

/* F_0D29D @ 0000:D29D (linear 0x0D29D, 679 bytes) */



static void near F_0D29D(void)
{
    struct Range *r;
    int old3;
    int vl;
    int d;
    int liml;
    int i5;
    int ticks;

    old3 = g_79e2.f0;
    _disable();
    ticks = g_78e0.fc0 - g_54d0;
    g_54d0 = g_78e0.fc0;
    _enable();
    r = &g_79e2.gear[g_79e2.f18].r[g_6cb8];
    for (i5 = 0; i5 < ticks; i5++) {
        if (g_terrain2[g_7e8a] != g_terrain2[g_7e8a + 1]) {
            if (g_54d6 == 0) {
                g_54d6 = 1;
                vl = g_6cac - (g_7e8a > g_7d30 ? g_7d84->f2 : g_7d84->f2 >> 1);
                if (vl < g_79e2.f0)
                    F_0D1DE(vl, &g_79e2);
            }
            continue;
        }
        g_54d6 = 0;
        d = g_7e8a - g_7d30;
        if (d > 12 && (g_7ec0 != 1 || g_7d40 == 0)) {
            if (g_78e0.f0 >= 0x61)
                vl = g_6cb0;
            else if (g_7d40)
                vl = g_6cb0;
            else
                vl = g_6cb2;
        chk:
            if (vl < g_79e2.f0) {
                F_0D1DE(vl, &g_79e2);
                continue;
            }
        } else if (d > -5 && d < 5) {
            vl = g_78e0.f0 + g_6cae;
            if (g_7d30 >= 30 && vl < g_6cb2)
                vl = g_6cb2;
            goto chk;
        }
        if (g_7f88 >= 2 && g_78e0.f0 > 50 && g_79e2.f0 > g_78e0.f0 + 32)
            F_0D1DE(g_78e0.f0 + 32, &g_79e2);
        else {
            g_79e2.fe += g_79e2.f14;
            if (g_79e2.fe > g_6cba)
                g_79e2.fe = g_6cba;
            if (g_79e2.fe > g_79e2.f12) {
                g_79e2.f42++;
                g_79e2.f12 = (r = &g_79e2.gear[g_79e2.f18].r[g_79e2.f42])->f2;
                g_79e2.f14 = r->f4;
            }
            if (g_79e2.f18 < g_79e2.f1c &&
                ((g_79e2.f24 && g_79e2.fe >= g_79e2.f22) || g_79e2.fe >= g_6cba)) {
                g_79e2.f18++;
                if (g_79e2.f24 == 0)
                    g_6cba = g_79e2.f18 < g_79e2.f1c ? g_79e2.f1e - 100 * g_6cb4
                           : g_8acc < 4 && g_vid.mode != -6 ? 0x14b4 : 0x1518;
                g_79e2.f40 = g_79e2.f34[g_79e2.f18];
                g_79e2.fe = g_79e2.f0 * g_79e2.f40;
                if ((r = &g_79e2.gear[g_79e2.f18].r[g_6cb8 = 1])->f2 <= g_79e2.fe) {
                    g_6cb8 = 2;
                    r++;
                } else if (r->f0 > g_79e2.fe) {
                    g_6cb8 = 0;
                    r--;
                }
                g_79e2.f42 = g_6cb8;
                g_79e2.f12 = r->f2;
                g_79e2.f14 = r->f4;
            }
            g_79e2.f0 = g_79e2.fe / g_79e2.f40;
        }
    }
    if (g_7e8a >= g_6caa) {
        liml = g_7d30 + 5 < g_7e8a ? 5 : 10;
        g_79e2.f0 = old3;
        if (liml < old3)
            g_79e2.f0 -= liml;
        else
            g_79e2.f0 = 0;
    }
}

/* F_0D544 @ 0000:D544 (linear 0x0D544, 224 bytes) */


int F_0D544(int rpm)
{
    if (rpm <= 0)
        return 0;
    if (g_8acc < 4) {
        if (rpm <= 0x47)
            return (rpm >> 3) + 1;
        return (rpm >> 2) - 8;
    }
    if (rpm <= 0x61) {
        if (rpm <= 0x37) {
            if (rpm <= 0x1b) {
                if (rpm <= 0xd)
                    return 1;
                return 2;
            }
            if (rpm <= 0x29)
                return 3;
            return 4;
        }
        if (rpm <= 0x53) {
            if (rpm <= 0x45)
                return 5;
            return 6;
        }
        return 7;
    }
    if (rpm <= 0x71) {
        if (rpm <= 0x69) {
            if (rpm <= 0x65)
                return 8;
            return 9;
        }
        if (rpm <= 0x6d)
            return 10;
        return 11;
    }
    if (rpm <= 0x79) {
        if (rpm <= 0x75)
            return 12;
        return 13;
    }
    if (rpm <= 0x81) {
        if (rpm <= 0x7d)
            return 14;
        return 15;
    }
    return 16;
}
