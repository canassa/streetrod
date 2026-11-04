#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int by70c2[6];
int by70d2[6];
unsigned far *q_72ee;
int at72f8[13];
int far *af7560z;
int far *af7880z;
int bq7884[13];
int bn78ec;
int aw7aaex;
int al7ab0u[13];
int n_7ace[13];
int g7d38m;
int g_7d3a;
int ao7d3ex;
unsigned far *g7d62p;
int g7f22s[6];
int at7f8au[13];
int g7fd8e[13];
unsigned far *aw8634x;
int as8638y[6];
int aj864ax[6];
int ay865a[6];

/* F_27B6D @ 2645:171D (linear 0x27B6D, 82 bytes)

   Points the terrain pointers at entry n of each terrain array. */


void F_27B6D(int n)
{
    q_72ee = &g_terrain0[n];
    aw8634x = &g_terrain1[n];
    g7d62p = &g_terrain3[n];
    af7880z = &g_terrain4[n];
    af7560z = &g_terrain2[n];
}

/* F_27BBF @ 2645:176F (linear 0x27BBF, 66 bytes) */


int F_27BBF(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = a_7d36m - b;
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
    v = b - g7d38m;
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
    v = a_7d36m - b;
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
    v = b - g7d38m;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}

/* F_27CAD @ 2645:185D (linear 0x27CAD, 158 bytes)

   Resets the terrain scroll state for a new track. */


void F_27CAD(void)
{
    if (bw8aca)
        ac7d30z = 200;
    else
        switch (g_0284) {
        case 1:
            ac7d30z = 10;
            break;
        case 2:
            ac7d30z = 65;
            break;
        case 3:
            ac7d30z = 115;
            break;
        }
    F_27B6D(ac7d30z);
    g7d82p = 1500;
    cz8bd0y = 3300;
    bp7d32 = 0;
    a_7d36m = 2560;
    g7d38m = 1900;
    g_7d3a = 0;
    ao7d3ex = 0;
    c_7d3c = 0;
    aa7d34l = 0;
    g_54d4 = 1;
    g_8648 = !random_int(2);
    gz7d7e = 1;
}

/* F_27D4B @ 2645:18FB (linear 0x27D4B, 721 bytes)

   Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. F_2801C is the backward twin. */


void F_27D4B(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p++ & 0xe0;
    p11 = n_7ace;
    p2 = at72f8;
    p3 = bq7884;
    p42 = at7f8au;
    p5l = g7fd8e;
    p6 = al7ab0u;
    rowz = 4;
    d1 = ao7d3ex - *af7880z++;
    d2 = c_7d3c - *af7560z++;
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
    rowz = 12 - bp7d32;
    k7 = 14;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p++;
        d1 = ao7d3ex - *af7880z;
        af7880z++;
        d2 = c_7d3c - *af7560z;
        af7560z++;
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

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p-- & 0xe0;
    p11 = ay865a;
    p2 = g7f22s;
    p3 = as8638y;
    p42 = by70c2;
    p5l = by70d2;
    p6 = aj864ax;
    rowz = 8;
    d1 = *af7880z-- - ao7d3ex;
    d2 = *af7560z-- - c_7d3c;
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
    rowz = bp7d32 + 8;
    k7 = 7;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p--;
        d1 = *af7880z - ao7d3ex;
        af7880z--;
        d2 = *af7560z - c_7d3c;
        af7560z--;
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

    F_27B6D(ac7d30z);
    F_27D4B();
    F_27B6D(ac7d30z);
    F_2801C();
    F_27B6D(ac7d30z);
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

    ac7d30z += bp7d32 >> 3;
    F_27B6D(ac7d30z);
    bp7d32 &= 7;
    bits7 = *g7d62p & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g7d82p = 2000;
            cz8bd0y = 0xaf0;
        } else if (bits7 & 0x20) {
            g7d82p = 2000;
            cz8bd0y = 0xce4;
        } else {
            g7d82p = 1500;
            cz8bd0y = 0xaf0;
        }
    } else {
        g7d82p = 1500;
        cz8bd0y = 0xce4;
    }
    bits7 = g7d62p[4] & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            al70c0x = 2000;
            ao7d2ev = 0xaf0;
        } else if (bits7 & 0x20) {
            al70c0x = 2000;
            ao7d2ev = 0xce4;
        } else {
            al70c0x = 1500;
            ao7d2ev = 0xaf0;
        }
    } else {
        al70c0x = 1500;
        ao7d2ev = 0xce4;
    }
    ao7d3ex = *af7880z;
    if (g_0610 != 0 || g_0612 != 0 || (a_7eaap.m7ec0 == 0 ? 0x17c : 0x3fc) < ac7d30z) {
        goal6 = 0x960;
        if (ar7e90 > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = a_7d36m - goal6;
        if (abs(diff) > 100)
            a_7d36m = goal6 + (goal6 < a_7d36m ? 100 : -100);
        else
            a_7d36m = goal6;
        if (abs(*af7560z - c_7d3c) > 0) {
            e_78e0.f4 = (c_7d3c - *af7560z) * 2;
            if (abs(e_78e0.f4) > 24)
                e_78e0.f4 = e_78e0.f4 > 0 ? 24 : -24;
        }
        c_7d3c = *af7560z;
    }
}

/* F_284BB @ 2645:206B (linear 0x284BB, 169 bytes)

   Returns 1 when the car at the current terrain index would hit something. */


int F_284BB(void)
{
    g_5e0e = 0;
    if (g7d82p - 0x104 < a_7d36m - 0x1e && a_7d36m + 0x46 < cz8bd0y + 0x104) {
        if (g7d82p > a_7d36m - 0x1e) {
            if ((g_terrain0[ac7d30z] & 0x1000) || (g_terrain0[ac7d30z - 1] & 0x1000))
                return 1;
            if (g_terrain3[ac7d30z + 1] & 0xf000)
                return 1;
            g_5e0e = 1;
        } else if (a_7d36m + 0x46 > cz8bd0y) {
            if ((g_terrain1[ac7d30z] & 0x1000) || (g_terrain1[ac7d30z - 1] & 0x1000))
                return 1;
            if (g_terrain3[ac7d30z + 1] & 0xf002)
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
        c_7d3c += 3;
    else
        c_7d3c -= 3;
}

/* F_2858D @ 2645:213D (linear 0x2858D, 272 bytes) */


#pragma intrinsic(abs)

void F_2858D(void)
{
    if (e_78e0.f2 && e_78e0.f0 && abs(c_7d3c - *af7560z - e_78e0.f2) < 200)
        c_7d3c -= (e_78e0.f2 >> 1) + 1;
    if (!bw8aca) {
        if (bl82c6)
            aa7d34l = 6;
        else
            aa7d34l = 4;
    }
    if (aa7d34l > 0 && aw7aaex > 0)
        aw7aaex -= 4;
    F_282EE();
    if (g_0610 == 0 && g_0612 == 0) {
        if (at787cu)
            a_7d36m += 25;
        else if (e_78e0.f0)
            a_7d36m -= (c_7d3c - *af7560z) * (aa7d34l + bn78ec) >> 2;
    }
    if (!bw8aca) {
        bp7d32 += aa7d34l;
        if (bp7d32 >= 8)
            F_2834C();
        if ((g_terrain0[ac7d30z + 7] & 1) || g_5e12) {
            g_5e12 = 1;
            F_28576(1);
        }
        if ((g_terrain1[ac7d30z + 6] & 1) || g_5e10) {
            F_28576(-1);
            g_5e10 = 1;
        }
    }
}
