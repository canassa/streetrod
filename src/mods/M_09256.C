#include <string.h>
#include "STRUCTS.H"
#include "M_0914C.H"
#include "GBSS.H"
#include "MESSAGES.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_0F38A.H"
#include "M_113A2.H"
#include "M_174A6.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Spec *ar7d7au;
struct Racer *g7d84o;
int bm7f86;

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

int g_51f8 = -1;   /* DS:51F8 */

/* F_092A4 @ 0000:92A4 (linear 0x092A4, 640 bytes) */



static void near F_092A4(int mode, int b, int c, int d, int e, int *f)
{
    int x, n, old;

    old = g_5a24;
    strcpy(g7f32s, g_2c02 + g7d84o->f6);
    strcat(g7f32s, "  ");
    strcat(g7f32s, g_2c02 + ar7d7au->f6);
    x = g_0284 == 6;
    if (mode == 4)
        g_6c84 = -1;
    bm7f86 = mode == 2;
    if (mode == 1 && !F_0914C(0, bm7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, 10))
        F_0914C(1, bm7f86, 20, 0);
    if (g7f32s[n = strlen(g7f32s) - 1] == '.')
        g7f32s[n] = 0;
    for (;;) {
        cz8bc0y = F_10B48(g7f32s);
        n = 320 - (ar82c0 + cz8bc0y);
        n = n > 0 ? n >> 1 : 0;
        n = n < 15 ? 15 : n;
        g_51ee = (n + ar82c0) & ~7;
        if (g_51ee + cz8bc0y > 320) {
            F_09256(g7f32s);
            continue;
        }
        break;
    }
    F_11421(-1, "", 0);
    F_11421(g_51ee, g7f32s, cz8bc0y);
    F_181C8(mode, b, g7d84o->fe, g7d84o->ff, c, d, e, f);
    if (old)
        mode = 2;
    if (mode == 2)
        g_51f0++;
    if (mode == 2) {
        g7f32s[0] = 0;
        F_11421(g_51ee, " ", cz8bc0y);
    }
    if (mode == 4)
        g_6c84 = 0;
    bm7f86 = (mode == 2) ? 0 : 1;
    if (!(x && bm7f86))
        if (!F_0914C(0, bm7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, bm7f86 == 0 ? 8 : 0))
            F_0914C(1, bm7f86, 40, 0);
}
