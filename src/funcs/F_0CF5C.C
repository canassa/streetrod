/* F_0CF5C @ 0000:CF5C (linear 0x0CF5C, 213 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0CDAF(struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);

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
