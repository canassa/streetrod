#include <string.h>
#include "STRUCTS.H"
#include "M_00D58.H"
#include "HOTSPOT.H"
#include "MESSAGES.H"
#include "M_02B9A.H"
#include "M_02DF4.H"
#include "M_0E56A.H"
#include "M_113A2.H"
#include "M_15236.H"

static int g_6c4c;                         /* DS:6C4C */
static int g_6c50a;                        /* DS:6C50 */

static struct Node * near F_03FF0(int idx, int a);

/* F_01BAE @ 0000:1bae (linear 0x01bae, 1076 bytes) */


/* Loop optimization is off from here to the end of the file. The file is
   compiled /Oalsi, but in the shipped code the loops of F_01BAE, F_01FE2 and
   F_0238C (the timer interrupt handler) are plain /Ol-off loops: counters and
   pointers stay in memory, the test is reached by a jmp, no induction
   variables. No for/while/do formulation reproduces that under /Ol (/Ol takes
   every such loop here); this one pragma reproduces all three functions. */
#pragma loop_opt(off)

void F_01BAE(void)
{
    int num;
    int cx;
    int k;
    int mask;
    int step;
    int cy;

    if (g_6c4c == 2)
        random_int(-1);
    if (g_06a6) {
        g_47cc |= 0x80;
        g_06a6 = 0;
        F_02E93();
        return;
    }
    if (!g_06a7) {
        if (g_06ac && g_6c4c == 10) {
            g_06ac = 0;
            g_06a8 = 15;
        }
        num = (int)strlen((char *)g_579f[g_6c4c]) / 2;
        k = 0;
        switch (g_6c4c) {
        case 14:
        case 18:
        case 19:
        case 22:
        case 36:
            num -= 2;
            break;
        case 11:
            num -= 2;
        case 37:
            k = 1;
            break;
        case 23:
            g_06a8 = 4;
            break;
        case 8:
            k = 4;
            break;
        case 28:
        case 35:
            k = 2;
            break;
        case 9:
            if (a_7eaap.m7eb0)
                break;
        case 13:
            num--;
            break;
        }
        if (g_6c4c == 10 && g_06aa != 10) {
            g_6c50a = num - g_06a8++ - 1;
            g_06a8 = g_06a8 % num;
        } else
            g_6c50a = random_int(num + k);
        switch (g_6c4c) {
        case 21:
            if ((g_6c50a >= 0 && g_6c50a < 9) || g_6c50a == 12)
                g_6c50a = 9;
            if (g_06aa == 28)
                g_6c50a = 11;
            break;
        case 20:
            g_6c50a = g_06b4[g_06b2++];
            if (g_6c50a == 2)
                g_06b2 = 0;
            break;
        case 23:
            if (g_6c50a)
                break;
        case 2:
            g_6c50a = 1;
            break;
        case 4:
            if (g_06a8 < 11)
                g_6c50a = 0;
            else
                g_6c50a = num - 1;
            break;
        case 34:
        case 41:
            g_6c50a = 0;
            break;
        case 36:
            if (g_06aa == 36)
                g_6c50a = 0;
            break;
        case 11:
            if (g_06aa == 11)
                g_6c50a = 10;
            break;
        case 27:
            g_6c50a = 5;
            break;
        case 7:
            if (a_7eaap.m7eb0 == 0)
                g_6c50a = 0;
            break;
        }
        if (g_6c4c == 8)
            g_06ae = 160;
        if (g_6c50a >= 0) {
            if (num > g_6c50a) {
                g_47cc = g_579f[g_6c4c][g_6c50a].key;
                mask = g_579f[g_6c4c][g_6c50a].val + 0x32;
                step = g_2bad[g_6c4c];
                for (; step - g_2bad[g_6c4c] < 32 && g_09f4[step].f6 != mask; step++)
                    ;
                if (g_09f4[step].f6 == mask) {
                    cx = (g_09f4[step].x1 + g_09f4[step].x2) / 2;
                    if (cx)
                        g_06ae = cx;
                    if (g_6c4c == 8)
                        g_06ae = 160;
                    cy = (g_09f4[step].y1 + g_09f4[step].y2) / 2;
                    if (cy)
                        g_06b0 = cy;
                }
            } else {
                if (num == g_6c50a)
                    g_47cc = 0x1c;
                if (num + 1 == g_6c50a)
                    g_47cc = 0x39;
                if (num + 2 == g_6c50a)
                    g_47cc = 0x48;
                if (num + 3 == g_6c50a)
                    g_47cc = 0x50;
                if (num + 4 == g_6c50a)
                    g_47cc = 0x4b;
                if (num + 5 == g_6c50a)
                    g_47cc = 0x4d;
            }
        } else
            g_47cc = 0x4c;
    }
    if ((g_06ae && g_472a != g_06ae) || (g_06b0 && g_472c != g_06b0)) {
        g_06a7 = 1;
        return;
    }
    g_06a7 = 0;
    if (g_6c4c == 0x25 && g_06ac < 10 && g_47cc == 0x22)
        g_47cc = 0x1c;
    if (g_6c4c == 10 && g_06aa == 10) {
        g_47cc = 0x1c;
        g_06aa = 0;
    } else
        g_06aa = g_6c4c;
    if (a_7eaap.m7eb8 == 0 && g_6c4c == 7)
        F_03FF0(0x2a, g_4806[42].f0);
    if (a_7eaap.m7eb0 == 0) {
        if (g_6c4c == 10)
            g_47cc = 0x31;
        if (g_6c4c == 8 && (g_47cc == 1 || g_47cc == 0x22 || g_47cc == 0x19))
            g_47cc = 0x39;
    }
    if (g_47cc == 0x46)
        g_47cc = 0x1c;
    g_06a6 = 1;
    F_02E93();
}
