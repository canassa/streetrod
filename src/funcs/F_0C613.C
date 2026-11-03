/* F_0C613 @ 0000:C613 (linear 0x0C613, 1948 bytes) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0C1D8(void);
static void near F_0C497(void);
static void near F_0C52C(int n);
static void near F_0C5AC(int n);
static int near F_0BED4(void);
static void near F_0BF94(void);
static struct Car * near F_04036(int idx, int weight, struct Car *old);
static int near F_043BF(struct Car *c, int keep);
static void near F_0DCBE(struct Car *c, struct Eng *e);

static int near F_0C613(void)
{
    int units;
    char opt;
    int sel;
    int kind;
    unsigned v;
    char stop;
    char *ptr;
    char temp[40];
    char engine;
    int all;
    int i;
    long cost;
    struct Car *found;
    int list[25];

    kind = g_79a2;
    v = g_79a4;
    engine = g_799c & 4;
    opt = g_799c & 0x10;
    stop = g_799c == 5 || g_799c == 0x11 || g_799c == 0x15;
    if (g_7eb0->slot6->b > 9900)
        g_7eb0->slot6->b = 9900;
    if (g_7eb0->rack.s[0]->b > 9900)
        g_7eb0->rack.s[0]->b = 9900;
    if (g_7eb0->slot12->b > 9900)
        g_7eb0->slot12->b = 9900;
    g_7676 = 0;
    g_53e6 = 0;
    g_82bc = 0;
    g_47cd = 0;
    g_4732 = 0;
    if (g_7900)
        g_7904 = 1;
    if (g_787c)
        g_799c |= 8;
    F_13DE6(1);
    i = g_7ec0 == 0 ? 5 : 15;
    if (g_7eb0->f22 > i)
        g_7eb0->f22 -= i;
    else
        g_7eb0->f22 = 0;
    (*g_present)();
    if (g_7a9e & 1) {
        g_7676 = 1;
        F_0C497();
        F_120FE(-2);
        F_03AB8(g_7d40 == 0 ? (i = g_53c8[kind]) : g_7ec0 == 0 ? 0x14bd : 0x14d6);
        F_11540(v);
        if (kind == 5) {
            found = F_04036(g_7648, g_7d7a->f0, 0);
            found->slot12->b = random_int(500);
            found->f24 = ((g_7d84->ff & 0x1f) << 8) | (found->f24 & 0xe0ff);
            found->f24 = (((g_7d84->fe & 4) != 0 & 1) << 13) | (found->f24 & 0xdfff);
            found->f24 = (((g_7d84->fe & 2) != 0 & 1) << 14) | (found->f24 & 0xbfff);
            found->f24 = (((g_7d84->fe & 1) != 0 & 1) << 15) | (found->f24 & 0x7fff);
            found->f4 = g_7d84->fa;
            if (found->f5 > g_7eb0->f5) {
                g_7eb0->f26 = g_7eb2->f26;
                g_7eb2 = g_7eb0;
                g_7eb0 = found;
                F_0DCBE(g_7eb0, &g_78e0);
            }
            if (g_0610 == 0 || g_7f82 != g_4ff4) {
                if (g_7d84->f10 != 0 || g_7d40 != 0)
                    g_7d84->fb = 0xff;
                else {
                    for (i = 0, units = 0; i < 25; i++)
                        if (g_7d88[i].kind < g_7d7a->f2)
                            list[++units] = i;
                    if (units > 0) {
                        list[0] = units;
                        F_0542E(g_7f82, list);
                        g_7d84->f10 = 1;
                    } else
                        g_7d84->fb = 0xff;
                }
            }
        }
        F_13F21(1);
        if (g_799c & 8)
            if ((i = F_0BED4()) != 0)
                goto ret2;
        if (kind == 5)
            return 1;
        return 3;
    } else if (g_799c & 0x40) {
        F_16AA8(0);
        F_0BF94();
        i = (g_799c & 8) ? F_0BED4() : 0;
        if (g_78e0.f0 > 0x50)
            i += 0x4b;
        else if (g_78e0.f0 > 0x32)
            i += 0x32;
        else
            i += 0x14;
        F_13DE6(0x18);
        F_01417(0);
        sprintf(temp, "You have been fined $%2d for speeding", i);
        F_11421(-1, temp, 0x140 - g_8250);
        F_03A83(0xfa);
        if (i <= g_7eaa) {
            F_11540(-i);
            cost = g_05cc;
            F_03A83(g_05dc);
            g_05cc = cost;
            F_13F21(1);
            F_11421(-1, "", 0x140 - g_8250);
        } else {
            F_03AB8(0x1793);
            F_13F21(1);
            F_13F21(1);
            F_11540(-g_7eaa);
            F_11421(-1, " ", 0x140 - g_8250);
            return 4;
        }
    } else if (g_799c == 0x81) {
        F_120FE(-2);
        F_03AB8(0xecb2);
    } else {
        if (kind == 5) {
            F_043BF(g_7eb0, 0);
            g_7eb0 = 0;
        }
        F_11540(-v);
        if (g_799c == 1) {
            F_0C497();
            F_120FE(-2);
            i = kind == 5 ? 0x143a : 0x1427;
            F_03AB8(g_7d40 == 0 ? i : g_7ec0 == 0 ? 0x1450 : 0x146c);
            F_13F21(1);
            if (kind != 5)
                return 3;
ret2:
            return 2;
        } else if (stop) {
            F_120FE(-2);
            if (engine)
                F_0C52C(kind == 5 ? 0x16bd : 0x1634);
            else if (opt)
                F_0C5AC(kind == 5 ? 0x1678 : 0x1608);
        } else {
            F_0C1D8();
            if (kind == 5) {
                if (g_799c & 8) {
                    F_03AB8(0x16f6);
                    F_0BED4();
                } else
                    F_03AB8(0x1658);
            } else if (g_78e0.f0 > 0x5a) {
                v = F_043BF(g_7eb0, 0) / 10 + 0x13;
                g_7eb0 = 0;
                F_11540(v);
                ptr = g_4115;
                while (*ptr++ != '$')
                    ;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                F_00E24();
                F_03AB8(0x1513);
            } else {
                if (g_78e0.f0 > 0x3c) {
                    i = 0;
                    v = random_int(0x28) + 100;
                } else if (g_78e0.f0 > 0x1e) {
                    i = 1;
                    v = random_int(0x14) + 100;
                } else {
                    if (g_799c & 8) {
                        F_03AB8(0x1590);
                        all = F_0BED4();
                        goto fin;
                    }
                    F_03AB8(0x1561);
                    F_13F21(1);
                    return 1;
                }
                ptr = strchr(i ? g_2b09 : g_2ab2, '$') + 1;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                all = g_799c & 8;
                if (all)
                    strcpy(g_2aea, "Plus tires !");
                else
                    g_2aea[0] = 0;
                sel = i ? 0x1a : 0x19;
                if ((long)v > g_7eaa) {
                    g_1ac2 = g_1b40 = 0xe;
                    g_579f[sel][0].val = 0xd0;
                    g_579f[sel][1].key = 1;
                    g_579f[sel][1].val = 0xd0;
                } else {
                    g_1ac2 = g_1b40 = 0xa;
                    g_579f[sel][0].val = 0xcf;
                    g_579f[sel][1].key = 0x21;
                    g_579f[sel][1].val = 0xcf;
                }
                F_13DE6(sel);
                F_00E24();
                while ((i = F_01417(3000)) == 0)
                    ;
                F_13F21(1);
                switch (i) {
                case 1:
                    F_11540(-v);
                    if (all)
                        F_0BED4();
                    F_13F21(1);
                    return 1;
                case 2:
                    F_11540((int)(g_7eb0->weight * 0.4));
                    g_7eb0 = 0;
                    break;
                default:
                    return;
                }
            }
        }
fin:
        F_13F21(1);
        goto ret2;
    }
    F_13F21(1);
    return 3;
}
