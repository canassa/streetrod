/* F_29170 @ 2645:2D20 (linear 0x29170, 523 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2914C(int x1, int y1, int x2, int y2);

static void near F_29170(int col, int a, int b, int c, int d, int e, int f)
{
    int mode;
    int dxa;
    int dxb;
    int racc6;
    int laccl;
    int xr5;
    int xlk;
    int ymin2;
    int y6;
    int ymax;

    if (c == d && f == e) {
        for (y6 = f; y6 <= d; y6++)
            (*g_78d6)(a, b, y6, col);
    } else {
        ymin2 = f > e ? e : f;
        ymax = c < d ? d : c;
        if (f < e && c > d) {
            mode = 2;
            xlk = a;
            racc6 = a << 4;
        } else if (e < f && d > c) {
            mode = 3;
            xr5 = b;
            xlk = b << 4;
        } else {
            if (f <= e && c <= d) {
                mode = 4;
                racc6 = a << 4;
            } else {
                mode = 5;
                racc6 = b << 4;
            }
            laccl = racc6;
        }
        dxa = F_2914C(a, f, b, e);
        dxb = F_2914C(a, c, b, d);
        y6 = ymin2;
        goto test;
    loop:
        switch (mode) {
        case 2:
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else if (y6 <= d)
                xr5 = b;
            else
                xr5 = (racc6 += dxb) >> 4;
            break;
        case 3:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            break;
        case 4:
            if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else
                xr5 = b;
            break;
        case 5:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else
                xlk = a;
            if (y6 > d)
                xr5 = (racc6 += dxb) >> 4;
            else
                xr5 = b;
            break;
        }
        if (xlk > xr5)
            (*g_78d6)(xr5, xlk, y6, col);
        else
            (*g_78d6)(xlk, xr5, y6, col);
        y6++;
    test:
        if (y6 <= ymax && y6 <= g_clip_ymax)
            goto loop;
    }
}
