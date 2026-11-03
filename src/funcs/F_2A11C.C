/* F_2A11C @ 2645:3CCC (linear 0x2A11C, 247 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_287D1(int color, int x, int y1, int y2);
static void near F_29FC9(int x, int y, int n, int align, int clip, int mode);
static void near F_29F3B(int x, int y, int n, int f, int idx);

static void near F_2A11C(int x1, int x2, int y1, int y2, int n, int clip, int flags)
{
    int j;

    for (j = *g_6e28 + 1; j; j--) {
        F_2646D();
        F_287D1(10, x1 - j, y1, y2);
        F_287D1(10, x2 + j, y1, y2);
        F_26458();
        (*g_78d6)(x1, x2, y1 + j - 1, 10);
    }
    if (flags & 0x1000)
        F_29F3B((x1 + x2) >> 1, y1, n, 1, 13);
    else if (flags & 0x2000) {
        F_2646D();
        F_29FC9((x1 + x2) >> 1, y1, n, 0, clip, 1);
        F_26458();
    } else if (*g_8b82 & 1) {
        F_2646D();
        F_29FC9((x1 + x2) >> 1, y1, n, 0, clip, (flags & 0x4000) ? 2 : 3);
        F_26458();
    } else
        F_29F3B((x1 + x2) >> 1, y1, n, 1, (flags & 0x4000) ? 12 : 14);
}
