#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_09256(char *s);
static int near F_0914C(int mode, int arg, int pct1, int pct2);

/* F_09256 @ 0000:9256 (linear 0x09256, 78 bytes) */


static void near F_09256(char *s)
{
    int i;

    while ((i = strlen(s) - 1) >= 0) {
        if (s[i] != ' ')
            s[i] = 0;
        else
            break;
    }
    if (i >= 0)
        s[i] = 0;
}

/* F_092A4 @ 0000:92A4 (linear 0x092A4, 640 bytes) */



static void near F_092A4(int mode, int b, int c, int d, int e, int *f)
{
    int x, n, old;

    old = g_5a24;
    strcpy(g_7f32, g_2c02 + g_7d84->f6);
    strcat(g_7f32, "  ");
    strcat(g_7f32, g_2c02 + g_7d7a->f6);
    x = g_0284 == 6;
    if (mode == 4)
        g_6c84 = -1;
    g_7f86 = mode == 2;
    if (mode == 1 && !F_0914C(0, g_7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, 10))
        F_0914C(1, g_7f86, 20, 0);
    if (g_7f32[n = strlen(g_7f32) - 1] == '.')
        g_7f32[n] = 0;
    for (;;) {
        g_8bc0 = F_10B48(g_7f32);
        n = 320 - (g_82c0 + g_8bc0);
        n = n > 0 ? n >> 1 : 0;
        n = n < 15 ? 15 : n;
        g_51ee = (n + g_82c0) & ~7;
        if (g_51ee + g_8bc0 > 320) {
            F_09256(g_7f32);
            continue;
        }
        break;
    }
    F_11421(-1, "", 0);
    F_11421(g_51ee, g_7f32, g_8bc0);
    F_181C8(mode, b, g_7d84->fe, g_7d84->ff, c, d, e, f);
    if (old)
        mode = 2;
    if (mode == 2)
        g_51f0++;
    if (mode == 2) {
        g_7f32[0] = 0;
        F_11421(g_51ee, " ", g_8bc0);
    }
    if (mode == 4)
        g_6c84 = 0;
    g_7f86 = (mode == 2) ? 0 : 1;
    if (!(x && g_7f86))
        if (!F_0914C(0, g_7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, g_7f86 == 0 ? 8 : 0))
            F_0914C(1, g_7f86, 40, 0);
}
