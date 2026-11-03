/* F_0D992 @ 0000:D992 (linear 0x0D992, 147 bytes) */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0D1DE(int rpm, struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);
static int near F_0E218(int n);

static void near F_0D992(void)
{
    int n1;
    int old;

    old = g_79e2.f0;
    g_7d84 = &g_7ff8[10];
    F_0E0E2(0x19, &g_79e2);
    g_6cb4 = g_7d84->f1;
    F_0D1DE(old, &g_79e2);
    F_08D9C(5);
    for (n1 = 0; n1 < 12; n1++)
        g_8ace[n1] = g_7526[n1];
    g_6cac = F_0E218(0x19);
    g_6cb2 = (int)(g_6cac * 0.6);
    g_6cb0 = (int)(g_6cac * 0.8);
}
