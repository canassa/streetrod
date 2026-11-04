#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>
#include <stdlib.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct State8272 m_8272;

static void near F_068E8(int n, int x, int y);
static void near F_068B3(void);
static void near F_06882(void);
static int near F_06AD8(void);

/* F_06827 @ 0000:6827 (linear 0x06827, 91 bytes) */


int F_06827(int id)
{
    int n, k, base;

    base = ((unsigned)g_121c - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    for (k = 0; k < n; k++)
        if (g_09f4[base + k].f6 == id)
            return base + k;
}

/* F_06882 @ 0000:6882 (linear 0x06882, 49 bytes) */


static void near F_06882(void)
{
    int base, n;

    base = ((unsigned)g_121c - (unsigned)g_09f4) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}

/* F_068B3 @ 0000:68B3 (linear 0x068B3, 53 bytes) */


static void near F_068B3(void)
{
    int i;

    for (i = 0; i < 5; i++)
        g_09f4[F_06827(g_508a[i])].f0 = 14;
}

/* F_068E8 @ 0000:68E8 (linear 0x068E8, 446 bytes) - rect + hotspot setup for the 5 button rects of panel n */


static void near F_068E8(int n, int x, int y)
{
    struct Panel *panel;
    struct Hot *hotspot;

    panel = &ai70e4t[n];
    hotspot = &g_09f4[F_06827(11)];
    hotspot->x1 = panel->r[0].x + x;
    hotspot->y1 = panel->r[0].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[0].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[0].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(13)];
    hotspot->x1 = panel->r[1].x + x;
    hotspot->y1 = panel->r[1].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[1].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[1].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(9)];
    hotspot->x1 = panel->r[2].x + x;
    hotspot->y1 = panel->r[2].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[2].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[2].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(10)];
    hotspot->x1 = panel->r[3].x + x;
    hotspot->y1 = panel->r[3].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[3].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[3].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(12)];
    hotspot->x1 = panel->r[4].x + x;
    hotspot->y1 = panel->r[4].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[4].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[4].h;
    hotspot->f0 = 11;
}

/* F_06AA6 @ 0000:6AA6 (linear 0x06AA6, 50 bytes) */


void F_06AA6(void)
{
    char *s;

    s = "                                ";
    strcpy(g_29ff, s);
}

/* F_06AD8 @ 0000:6AD8 (linear 0x06AD8, 46 bytes) */


static int near F_06AD8(void)
{
    if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2 == 0 && a_7eaap.m7eb8 == 0 && a_7eaap.m7eaa < 400L)
        return 1;
    return 0;
}

char g_50c2[6][2] = {{4, 12}, {3, 11}, {2, 10}, {5, 13}, {1, 9}, {7, 15}};   /* DS:50C2 */
int g_50ce[7] = {0, 5, 5, 5, 5, 0, 0};   /* DS:50CE */

/* F_06B06 @ 0000:6B06 (linear 0x06B06, 1125 bytes) */



static int near F_06B06(int mode)
{
    char *p;
    int r;
    int n;

    if (g_4733 > 0)
        F_120FE(-4);
    F_06529(0);
    if (g_0282) {
        if (g_0282 & 1)
            F_120FE(-2);
        return 0x29a;
    }
    F_120FE(0);
    F_11421(-1, "", 0);
    g_0284 = 1;
    switch (mode) {
    case 3:
        if (g_4f48 == 0) {
            F_120FE(-2);
            F_03AB8(0x1867);
            F_120FE(-4);
            break;
        }
        if (g_4f46) {
            n = F_049A2(g_4f46);
            F_181C8(6, g_4f46->f2, n, (g_4f46->f24 >> 8) & 0x1f,
                    0, 0x98, 0, g_5030[((struct Row8 *)((char *)g_4806 + 2))[g_4f46->slot12->idx].kind]);
            F_06AA6();
            F_13DE6(10);
            F_13F21(1);
        }
    case 2:
        r_vid.f16 = 0;
    case 1:
        if (mode == 3)
            mode = 1;
        else {
            F_118D4(0x408, -1);
            F_10B7C();
            if (mode == 2 && a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2) {
                a_7eaap.m7eb0 = a_7eaap.m7eb2;
                a_7eaap.m7eb2 = a_7eaap.m7eb0->f26;
                mode = 1;
            }
        }
        r_vid.f16 = 1;
        (*m_present)();
        if (a_7eaap.m7eb0) {
            m_8272.m827a = w_7d88[a_7eaap.m7eb0->f2].f6;
            m_8272.m827c = a_7eaap.m7eb0->f4;
            m_8272.m827e = ((struct Row8 *)((char *)g_4806 + 2))[a_7eaap.m7eb0->slot12->idx].kind;
            if (mode == 1)
                F_067CF(m_8272.m827c);
            m_8272.m8280 = 0;
            m_8272.m8282 = a_7eaap.m7eb0->f2;
            m_8272.m8284.shade = (a_7eaap.m7eb0->f24 >> 8) & 0x1f;
            m_8272.m8284.roof = (a_7eaap.m7eb0->f24 >> 13) & 1;
            m_8272.m8284.front = (a_7eaap.m7eb0->f24 >> 14) & 1;
            m_8272.m8284.rear = (a_7eaap.m7eb0->f24 >> 15) & 1;
            for (r = 0; r < 5; r++)
                o_8290[r] = a_7eaap.m7eb0->rack.s[r]->idx;
            n = F_049A2(a_7eaap.m7eb0);
            strcpy(g_29ff, g_2c02 + w_7d88[a_7eaap.m7eb0->f2].f4);
            if (mode == 2)
                F_181C8(3, m_8272.m8282, n, m_8272.m8284.shade, 0, 0x98, 0, g_5030[m_8272.m827e]);
            else
                F_181C8(4, m_8272.m8282, n, m_8272.m8284.shade, 0, 0x98, 0, g_5030[m_8272.m827e]);
        } else {
            F_1A92A(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xc7);
            g_29ff[0] = 0;
        }
        break;
    case 0:
        break;
    }
    if (a_7eaap.m7eb0)
        F_068E8(m_8272.m8282, ac825et, aw8260y);
    else
        F_068B3();
    F_06882();
    F_11593(g_49dc);
    F_120FE(-2);
    if (a_7eaap.m7eb6 > 15)
        F_03AB8(0x0d30);
    else if (a_7eaap.m7ebc > 130) {
        if (a_7eaap.m7ebc == 131)
            F_03AB8(0x0d76);
        else {
            p = g_39c9;
            p += strlen(p) + 1;
            p += strcspn(p, "1234567890");
            itoa(a_7eaap.m7ebc - 130, p, 10);
            p[strlen(p)] = ' ';
            F_03AB8(0x0dc7);
        }
    }
    if (F_06AD8()) {
        F_03AB8(0x17ff);
        g_0282 |= 0x10;
    } else {
        F_16381(0);
        F_00E24();
        r = F_14294(10);
    }
    F_06529(0);
    if (g_0282 & 0x10)
        g_0282 = 0x10;
    if (g_0282) {
        r = 0x29a;
        if ((g_0282 & 1) == 0)
            F_120FE(-4);
    }
    return r;
}

/* F_06F6B @ 0000:6F6B (linear 0x06f6b, 431 bytes) */


static void near F_06F6B(void)
{
    struct Bitmap far *bm1;
    int x;
    int idx4;
    int y;
    int key;

    if (!a_7eaap.m7eb0)
        F_03AB8(0x0efa);
    else if (r_vid.mode != -2 && r_vid.mode != -6)
        F_03AB8(0x18ad);
    else {
        bm1 = F_14090(14, &x, &y, 2);
        F_13DE6(14);
        idx4 = a_7eaap.m7eb0->f4;
        for (;;) {
            F_112CB(12, g_50c2[idx4][0]);
            while ((key = F_01417(3000)) == 0)
                ;
            switch (key) {
            case 1:
                if (--idx4 < 0)
                    idx4 = 5;
                continue;
            case 2:
                if (++idx4 >= 6)
                    idx4 = 0;
                continue;
            case 3:
                if (a_7eaap.m7eaa < 20L) {
                    F_03AB8(0x10a1);
                    break;
                }
                r_vid.f16 = 1;
                F_13F21(1);
                F_11BD3(bm1, x, y);
                F_19438(1);
                F_11BD3(g_0294, m_8272.m8272, m_8272.m8274);
                a_7eaap.m7eb0->f24 &= 0xe0ff;
                F_067CF(a_7eaap.m7eb0->f4 = idx4);
                F_112CB(12, 5);
                g_05fc += 0x888L;
                F_11540(-20);
                return;
            case -23:
            default:
                break;
            }
            break;
        }
        F_13F21(1);
        F_112CB(12, 5);
        F_11BD3(bm1, x, y);
        F_19438(1);
    }
}

/* F_0711A @ 0000:711A (linear 0x0711a, 589 bytes) */


static void near F_0711A(unsigned mask)
{
    struct Bitmap far *bm1;
    int n9;
    char *msg;
    unsigned val5;
    int x;
    int r7;
    struct Model *p9;
    int y;
    int key;
    int cost;

    p9 = &w_8df0[a_7eaap.m7eb0->f2];
    if (!a_7eaap.m7eb0) {
        F_03AB8(0x0efa);
        return;
    }
    if (mask & 4) {
        if (p9->f2 == 0) {
            F_03AB8(0x1914);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x2000) ? (g_239e + 1299) : (g_239e + 1283);
        cost = 70;
        val5 = 0x2220;
    }
    if (mask & 2) {
        if (p9->f6 == 0) {
            F_03AB8(0x1936);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x4000) ? (g_239e + 1337) : (g_239e + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if (mask & 1) {
        if (p9->f8 == 0) {
            F_03AB8(0x1936);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x8000) ? (g_239e + 1337) : (g_239e + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if ((long)cost > a_7eaap.m7eaa) {
        F_03AB8(0x1959);
        return;
    }
    bm1 = F_14090(34, &x, &y, 2);
    g_1e38 = msg - g_239e;
    F_13DE6(34);
    while ((key = F_01417(3000)) == 0)
        ;
    switch (key) {
    case -20:
        if (mask & 4)
            a_7eaap.m7eb0->f24 ^= 0x2000;
        if (mask & 2)
            a_7eaap.m7eb0->f24 ^= 0x4000;
        if (mask & 1)
            a_7eaap.m7eb0->f24 ^= 0x8000;
        F_120FE(-4);
        F_13F21(1);
        F_11BD3(bm1, x, y);
        F_19438(1);
        n9 = a_7eaap.m7eb0->f2;
        r7 = F_049A2(a_7eaap.m7eb0);
        F_181C8(8, n9, r7, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                g_5030[((struct Row8 *)((char *)g_4806 + 2))[a_7eaap.m7eb0->slot12->idx].kind]);
        F_16BC1();
        F_120FE(-2);
        g_05fc += val5;
        F_11540(-cost);
        break;
    case -22:
        F_13F21(1);
        F_11BD3(bm1, x, y);
        F_19438(1);
        break;
    }
}
