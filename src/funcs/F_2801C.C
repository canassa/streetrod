/* F_2801C @ 2645:1BCC (linear 0x2801C, 722 bytes)

   Fills the per-row projection tables (8 rows, the first one peeled) from the
   terrain pointers, walking the track backward. F_27D4B is the forward twin. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_2801C(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g_7d62 & 1) || (*g_7d62 & 0xf000);
    bits = *g_7d62-- & 0xe0;
    p11 = g_865a;
    p2 = g_7f22;
    p3 = g_8638;
    p42 = g_70c2;
    p5l = g_70d2;
    p6 = g_864a;
    rowz = 8;
    d1 = *g_7880-- - g_7d3e;
    d2 = *g_7560-- - g_7d3c;
    *p3++ = F_27C78(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = F_27C3D(rowz, 0xaf0, d2);
            *p2++ = F_27C3D(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = F_27C3D(rowz, 0xce4, d2);
            *p2++ = F_27C3D(rowz, 2000, d2);
        } else {
            *p11++ = F_27C3D(rowz, 0xaf0, d2);
            *p2++ = F_27C3D(rowz, 1500, d2);
        }
    } else {
        *p11++ = F_27C3D(rowz, 0xce4, d2);
        *p2++ = F_27C3D(rowz, 1500, d2);
    }
    *p5l++ = F_27C3D(rowz, 0xe74, d2);
    *p42++ = F_27C3D(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = F_27C78(rowz, 0x76c, d1);
    else
        p6++;
    rowz = g_7d32 + 8;
    k7 = 7;
    do {
        flag8 = (*g_7d62 & 1) || (*g_7d62 & 0xf000);
        bits = *g_7d62 & 0xe0;
        g_7d62--;
        d1 = *g_7880 - g_7d3e;
        g_7880--;
        d2 = *g_7560 - g_7d3c;
        g_7560--;
        if (bits) {
            if (bits & 0x80) {
                *p11 = F_27C3D(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27C3D(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = F_27C3D(rowz, 0xce4, d2);
                p11++;
                *p2 = F_27C3D(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = F_27C3D(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27C3D(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = F_27C3D(rowz, 0xce4, d2);
            p11++;
            *p2 = F_27C3D(rowz, 1500, d2);
            p2++;
        }
        *p3 = F_27C78(rowz, 2000, d1);
        p3++;
        *p5l = F_27C3D(rowz, 0xe74, d2);
        p5l++;
        *p42 = F_27C3D(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = F_27C78(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}
