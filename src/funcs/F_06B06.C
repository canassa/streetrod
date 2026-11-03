/* F_06B06 @ 0000:6B06 (linear 0x06B06, 1125 bytes) */

#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_068E8(int n, int x, int y);
static void near F_068B3(void);
static void near F_06882(void);
static int near F_06AD8(void);

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
                    0, 0x98, 0, g_5030[g_4808[g_4f46->slot12->idx].kind]);
            F_06AA6();
            F_13DE6(10);
            F_13F21(1);
        }
    case 2:
        g_vid.f16 = 0;
    case 1:
        if (mode == 3)
            mode = 1;
        else {
            F_118D4(0x408, -1);
            F_10B7C();
            if (mode == 2 && g_7eb0 == 0 && g_7eb2) {
                g_7eb0 = g_7eb2;
                g_7eb2 = g_7eb0->f26;
                mode = 1;
            }
        }
        g_vid.f16 = 1;
        (*g_present)();
        if (g_7eb0) {
            g_827a = g_7d88[g_7eb0->f2].f6;
            g_827c = g_7eb0->f4;
            g_827e = g_4808[g_7eb0->slot12->idx].kind;
            if (mode == 1)
                F_067CF(g_827c);
            g_8280 = 0;
            g_8282 = g_7eb0->f2;
            g_8284.shade = (g_7eb0->f24 >> 8) & 0x1f;
            g_8284.roof = (g_7eb0->f24 >> 13) & 1;
            g_8284.front = (g_7eb0->f24 >> 14) & 1;
            g_8284.rear = (g_7eb0->f24 >> 15) & 1;
            for (r = 0; r < 5; r++)
                g_8290[r] = g_7eb0->rack.s[r]->idx;
            n = F_049A2(g_7eb0);
            strcpy(g_29ff, g_2c02 + g_7d88[g_7eb0->f2].f4);
            if (mode == 2)
                F_181C8(3, g_8282, n, g_8284.shade, 0, 0x98, 0, g_5030[g_827e]);
            else
                F_181C8(4, g_8282, n, g_8284.shade, 0, 0x98, 0, g_5030[g_827e]);
        } else {
            F_1A92A(g_vid.back, g_vid.front, 0, 0, 0x13f, 0xc7);
            g_29ff[0] = 0;
        }
        break;
    case 0:
        break;
    }
    if (g_7eb0)
        F_068E8(g_8282, g_825e, g_8260);
    else
        F_068B3();
    F_06882();
    F_11593(g_49dc);
    F_120FE(-2);
    if (g_7eb6 > 15)
        F_03AB8(0x0d30);
    else if (g_7ebc > 130) {
        if (g_7ebc == 131)
            F_03AB8(0x0d76);
        else {
            p = g_39c9;
            p += strlen(p) + 1;
            p += strcspn(p, "1234567890");
            itoa(g_7ebc - 130, p, 10);
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
