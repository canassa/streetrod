/* F_29398 @ 2645:2F48 (linear 0x29398, 773 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2914C(int x1, int y1, int x2, int y2);
static int near F_2937B(int n);

void F_29398(int n)
{
    int xl;
    int ybot6;
    int xr28;
    int yk;
    int wid;
    int cnt;
    int m17;
    int dxr7;
    int t1;
    int xlsn;
    int xr2s6;
    int dxl2k;
    int dxlm;
    int flagk;
    int dxr26;
    int t2;
    int xl2;
    int last2;
    int xrl;
    int cx0k;
    int xrs;
    int xl2s6;
    int m2;

    xl = *g_6e08;
    xrl = *g_6e18;
    xlsn = xl << 4;
    xrs = xrl << 4;
    yk = *g_6e1a;
    ybot6 = *g_6e1e;
    dxlm = F_2914C(*g_6e10, ybot6, xl, yk);
    dxr7 = F_2914C(*g_6e1c, ybot6, xrl, yk);
    flagk = *g_8b82 & 1;
    if (flagk) {
        xl2 = *g_6e0a;
        xr28 = *g_6e0e;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = F_2914C(*g_6e12, ybot6, xl2, yk);
        dxr26 = F_2914C(*g_6e16, ybot6, xr28, yk);
    }
    if (*g_8b82 & 8) {
        wid = *g_6e28;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (F_2937B(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 101; cnt--, yk++) {
        if (flagk) {
            if (*g_8b82 & 4)
                break;
            if (*g_6e20 & 0x80) {
                m17 = xl2 + ((xl - xl2) >> 1);
                (*g_78d6)(xl2, m17, yk, 4);
                (*g_78d6)(m17, xl, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xl2, m17, yk, 2);
            } else {
                (*g_78d6)(xl2, xl, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xl2, xl, yk, 2);
            }
            if (*g_6e24 & 0x80) {
                m2 = xr28 - ((xr28 - xrl) >> 1);
                (*g_78d6)(m2, xr28, yk, 4);
                (*g_78d6)(xrl, m2, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(m2, xr28, yk, 2);
            } else {
                (*g_78d6)(xrl, xr28, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xrl, xr28, yk, 2);
            }
        } else {
            (*g_78d6)(xl, xrl, yk, (*g_8b82 & 0x10) ? 15 : 1);
        }
        if (*g_8b82 & 8) {
            t1 = cx0k;
            t2 = t1 + wid;
            (*g_78d6)(t1, t2, yk, 4);
        }
        xlsn += dxlm;
        xrs += dxr7;
        xl = xlsn >> 4;
        xrl = xrs >> 4;
        if (flagk) {
            xl2s6 += dxl2k;
            xr2s6 += dxr26;
            xl2 = xl2s6 >> 4;
            xr28 = xr2s6 >> 4;
        }
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
}
