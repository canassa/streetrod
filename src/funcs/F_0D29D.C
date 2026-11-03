/* F_0D29D @ 0000:D29D (linear 0x0D29D, 679 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0D1DE(int rpm, struct Eng *p);

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
        if (g_ter4570[g_7e8a] != g_ter4570[g_7e8a + 1]) {
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
