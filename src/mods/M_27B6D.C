#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

/* F_27B6D @ 2645:171D (linear 0x27B6D, 82 bytes)

   Points the terrain pointers at entry n of each terrain array. */


void F_27B6D(int n)
{
    g_72ee = &g_terrain0[n];
    g_8634 = &g_terrain1[n];
    g_7d62 = &g_terrain3[n];
    g_7880 = &g_terrain4[n];
    g_7560 = &g_terrain2[n];
}

/* F_27BBF @ 2645:176F (linear 0x27BBF, 66 bytes) */


int F_27BBF(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_7d36 - b;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return 0xa0 - u;
}

/* F_27C01 @ 2645:17B1 (linear 0x27C01, 60 bytes) */


int F_27C01(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_7d38;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return u + 0x3c;
}

/* F_27C3D @ 2645:17ED (linear 0x27C3D, 59 bytes) */


int F_27C3D(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_7d36 - b;
    if (a == 0)
        a++;
    u = v / a;
    return 0x118 - u;
}

/* F_27C78 @ 2645:1828 (linear 0x27C78, 53 bytes) */


int F_27C78(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_7d38;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}

/* F_27CAD @ 2645:185D (linear 0x27CAD, 158 bytes)

   Resets the terrain scroll state for a new track. */


void F_27CAD(void)
{
    if (g_8aca)
        g_7d30 = 200;
    else
        switch (g_0284) {
        case 1:
            g_7d30 = 10;
            break;
        case 2:
            g_7d30 = 65;
            break;
        case 3:
            g_7d30 = 115;
            break;
        }
    F_27B6D(g_7d30);
    g_7d82 = 1500;
    g_8bd0 = 3300;
    g_7d32 = 0;
    g_7d36 = 2560;
    g_7d38 = 1900;
    g_7d3a = 0;
    g_7d3e = 0;
    g_7d3c = 0;
    g_7d34 = 0;
    g_54d4 = 1;
    g_8648 = !random_int(2);
    g_7d7e = 1;
}

/* F_27D4B @ 2645:18FB (linear 0x27D4B, 721 bytes)

   Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. F_2801C is the backward twin. */


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

/* F_2801C @ 2645:1BCC (linear 0x2801C, 722 bytes)

   Fills the per-row projection tables (8 rows, the first one peeled) from the
   terrain pointers, walking the track backward. F_27D4B is the forward twin. */


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

/* F_282EE @ 2645:1E9E (linear 0x282EE, 94 bytes)

   Rebuilds the projection tables for both directions, then waits 9 ticks. */


void F_282EE(void)
{
    long deadline;

    F_27B6D(g_7d30);
    F_27D4B();
    F_27B6D(g_7d30);
    F_2801C();
    F_27B6D(g_7d30);
    deadline = g_tick + 9;
    F_2AFDC();
    while (deadline > g_tick)
        ;
}

/* F_2834C @ 2645:1EFC (linear 0x2834C, 367 bytes) */


#pragma intrinsic(abs)

void F_2834C(void)
{
    int bits7, goal6, diff;

    g_7d30 += g_7d32 >> 3;
    F_27B6D(g_7d30);
    g_7d32 &= 7;
    bits7 = *g_7d62 & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g_7d82 = 2000;
            g_8bd0 = 0xaf0;
        } else if (bits7 & 0x20) {
            g_7d82 = 2000;
            g_8bd0 = 0xce4;
        } else {
            g_7d82 = 1500;
            g_8bd0 = 0xaf0;
        }
    } else {
        g_7d82 = 1500;
        g_8bd0 = 0xce4;
    }
    bits7 = g_7d62[4] & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g_70c0 = 2000;
            g_7d2e = 0xaf0;
        } else if (bits7 & 0x20) {
            g_70c0 = 2000;
            g_7d2e = 0xce4;
        } else {
            g_70c0 = 1500;
            g_7d2e = 0xaf0;
        }
    } else {
        g_70c0 = 1500;
        g_7d2e = 0xce4;
    }
    g_7d3e = *g_7880;
    if (g_0610 != 0 || g_0612 != 0 || (g_7ec0 == 0 ? 0x17c : 0x3fc) < g_7d30) {
        goal6 = 0x960;
        if (g_7e90 > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = g_7d36 - goal6;
        if (abs(diff) > 100)
            g_7d36 = goal6 + (goal6 < g_7d36 ? 100 : -100);
        else
            g_7d36 = goal6;
        if (abs(*g_7560 - g_7d3c) > 0) {
            g_78e0.f4 = (g_7d3c - *g_7560) * 2;
            if (abs(g_78e0.f4) > 24)
                g_78e0.f4 = g_78e0.f4 > 0 ? 24 : -24;
        }
        g_7d3c = *g_7560;
    }
}

/* F_284BB @ 2645:206B (linear 0x284BB, 169 bytes)

   Returns 1 when the car at the current terrain index would hit something. */


int F_284BB(void)
{
    g_5e0e = 0;
    if (g_7d82 - 0x104 < g_7d36 - 0x1e && g_7d36 + 0x46 < g_8bd0 + 0x104) {
        if (g_7d82 > g_7d36 - 0x1e) {
            if ((g_terrain0[g_7d30] & 0x1000) || (g_terrain0[g_7d30 - 1] & 0x1000))
                return 1;
            if (g_terrain3[g_7d30 + 1] & 0xf000)
                return 1;
            g_5e0e = 1;
        } else if (g_7d36 + 0x46 > g_8bd0) {
            if ((g_terrain1[g_7d30] & 0x1000) || (g_terrain1[g_7d30 - 1] & 0x1000))
                return 1;
            if (g_terrain3[g_7d30 + 1] & 0xf002)
                return 1;
            g_5e0e = 1;
        }
        return 0;
    }
    return 1;
}

/* F_28564 @ 2645:2114 (linear 0x28564, 18 bytes) */


void F_28564(void)
{
    g_5e10 = g_5e12 = 0;
    F_27CAD();
    F_2B6CA();
}

/* F_28576 @ 2645:2126 (linear 0x28576, 23 bytes) */


void F_28576(int dir)
{
    if (dir == 1)
        g_7d3c += 3;
    else
        g_7d3c -= 3;
}

/* F_2858D @ 2645:213D (linear 0x2858D, 272 bytes) */


#pragma intrinsic(abs)

void F_2858D(void)
{
    if (g_78e0.f2 && g_78e0.f0 && abs(g_7d3c - *g_7560 - g_78e0.f2) < 200)
        g_7d3c -= (g_78e0.f2 >> 1) + 1;
    if (!g_8aca) {
        if (g_82c6)
            g_7d34 = 6;
        else
            g_7d34 = 4;
    }
    if (g_7d34 > 0 && g_7aae > 0)
        g_7aae -= 4;
    F_282EE();
    if (g_0610 == 0 && g_0612 == 0) {
        if (g_787c)
            g_7d36 += 25;
        else if (g_78e0.f0)
            g_7d36 -= (g_7d3c - *g_7560) * (g_7d34 + g_78ec) >> 2;
    }
    if (!g_8aca) {
        g_7d32 += g_7d34;
        if (g_7d32 >= 8)
            F_2834C();
        if ((g_terrain0[g_7d30 + 7] & 1) || g_5e12) {
            g_5e12 = 1;
            F_28576(1);
        }
        if ((g_terrain1[g_7d30 + 6] & 1) || g_5e10) {
            F_28576(-1);
            g_5e10 = 1;
        }
    }
}
