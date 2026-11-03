/* F_27D4B @ 2645:18FB (linear 0x27D4B, 721 bytes)

   Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. F_2801C is the backward twin. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_27D4B(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g_7d62 & 1) || (*g_7d62 & 0xf000);
    bits = *g_7d62++ & 0xe0;
    p11 = g_7ace;
    p2 = g_72f8;
    p3 = g_7884;
    p42 = g_7f8a;
    p5l = g_7fd8;
    p6 = g_7ab0;
    rowz = 4;
    d1 = g_7d3e - *g_7880++;
    d2 = g_7d3c - *g_7560++;
    *p3++ = F_27C01(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = F_27BBF(rowz, 0xaf0, d2);
            *p2++ = F_27BBF(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = F_27BBF(rowz, 0xce4, d2);
            *p2++ = F_27BBF(rowz, 2000, d2);
        } else {
            *p11++ = F_27BBF(rowz, 0xaf0, d2);
            *p2++ = F_27BBF(rowz, 1500, d2);
        }
    } else {
        *p11++ = F_27BBF(rowz, 0xce4, d2);
        *p2++ = F_27BBF(rowz, 1500, d2);
    }
    *p5l++ = F_27BBF(rowz, 0xe74, d2);
    *p42++ = F_27BBF(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = F_27C01(rowz, 0x76c, d1);
    else
        p6++;
    rowz = 12 - g_7d32;
    k7 = 14;
    do {
        flag8 = (*g_7d62 & 1) || (*g_7d62 & 0xf000);
        bits = *g_7d62 & 0xe0;
        g_7d62++;
        d1 = g_7d3e - *g_7880;
        g_7880++;
        d2 = g_7d3c - *g_7560;
        g_7560++;
        if (bits) {
            if (bits & 0x80) {
                *p11 = F_27BBF(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27BBF(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = F_27BBF(rowz, 0xce4, d2);
                p11++;
                *p2 = F_27BBF(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = F_27BBF(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27BBF(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = F_27BBF(rowz, 0xce4, d2);
            p11++;
            *p2 = F_27BBF(rowz, 1500, d2);
            p2++;
        }
        *p3 = F_27C01(rowz, 2000, d1);
        p3++;
        *p5l = F_27BBF(rowz, 0xe74, d2);
        p5l++;
        *p42 = F_27BBF(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = F_27C01(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}
