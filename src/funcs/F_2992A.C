/* F_2992A @ 2645:34DA (linear 0x2992A, 725 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_29170(int mode, int a, int b, int c, int d, int e, int f);

static void near F_2992A(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b8l;
    int b4;
    int t8l;
    int y;

    t8l = *g_8b82 & 1;
    if (rev) {
        a41 = g_6e20[0] & 4;
        b8l = g_6e20[0] & 8;
        b4 = g_6e20[-1] & 4;
        c10 = g_6e20[0] & 0x10;
    } else {
        a41 = g_6e24[0] & 4;
        b8l = g_6e24[0] & 8;
        b4 = g_6e24[-1] & 4;
        c10 = g_6e24[0] & 0x10;
    }
    if (!(*g_8b82 & 4)) {
        if (c10)
            F_29170(2, a, b, c, d, e, f);
        else
            F_29170(3, a, b, c, d, e, f);
        if (a41) {
            if (b8l) {
                F_29170(4, a, b, f, e, e - d + e, f - c + f);
            } else {
                F_29170(4, a, b, f, e, e - ((d - e) >> 1), f - ((c - f) >> 1));
            }
        }
        if (g_8b82[-1] & 4) {
            if (rev)
                (*g_78d6)(g_clip_xmin, a, c - 1, 4);
            else
                (*g_78d6)(b, g_clip_xmax, d - 1, 4);
            if (rev) {
                for (y = f; y < c - 1; y++)
                    (*g_78d6)(g_clip_xmin, a, y, 2);
            } else {
                for (y = e; y < d - 1; y++)
                    (*g_78d6)(b, g_clip_xmax, y, 2);
            }
        }
        if (a41 && !b4) {
            int x1;
            int x2;

            if (b8l) {
                if (rev) {
                    y = f - c + f;
                    if (y < f) {
                        x1 = a - b + a;
                        x2 = a - 1;
                        do
                            (*g_78d6)(x1, x2, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - d + e;
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            } else {
                if (rev) {
                    y = f - ((c - f) >> 1);
                    if (y < f) {
                        x2 = a - b + a;
                        x1 = a - 1;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - ((d - e) >> 1);
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            }
        }
    }
}
