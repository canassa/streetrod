#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

/* F_07800 @ 0000:7800 (linear 0x07800, 241 bytes) */


static void near F_07800(void)
{
    struct Car *car;
    int sel;
    struct Node *tbl[142];
    char line[142];
    int y, cur;

    car = g_7eb0;
    if (car == 0) {
        F_03AB8(0x0efa);
        return;
    }
    F_076A7(4, tbl, line, 0);
    switch (F_142CF(4, 1, 0, g_49e0, line, 0)) {
    case -4:
        return;
    case -3:
        sel = F_14C8B();
        break;
    case 0x3ea:
        F_03AB8(0x0f4e);
        return;
    }
    y = g_4806[car->slot12->idx].f2;
    cur = g_4806[tbl[sel]->idx].f2;
    F_07741(F_077B7(tbl[sel]), &car->slot12);
    F_1B213(g_5030[y], g_5030[cur]);
    g_05fc += 0x444L;
}

/* F_078F1 @ 0000:78F1 (linear 0x078F1, 250 bytes) */


static void near F_078F1(int *ids, char *vals)
{
    int n;
    struct Item *it;
    int count;
    struct Node *node;
    struct Node *gear[7];

    for (n = 0; n < 5; n++)
        gear[n] = g_7eb0->rack.s[n];
    gear[n++] = g_7eb0->slot6;
    gear[n] = g_7eb0->slot12;

    for (count = 0, n = 0; n < 7; n++) {
        if (node = gear[n]) {
            count++;
            it = &g_4806[node->idx];
            ids[count] = g_50ce[n];
            g_49e0[count] = it->f6;
            if (node->b != -0x80)
                vals[count] = node->b / 100;
            else
                vals[count] = 0x80;
        }
    }
    for (n = 1; n <= 4; n++) {
        g_49e0[++count] = -n;
        vals[count] = 0x80;
        ids[count] = 0;
    }
    g_49e0[0] = count;
}

/* F_079EB @ 0000:79EB (linear 0x079EB, 185 bytes) */


void F_079EB(void)
{
    int ids[143];
    char vals[144];

    if (g_7eb0 == 0) {
        F_03AB8(0x0efa);
        return;
    }
    F_078F1(ids, vals);
    sprintf(g_511e[3] + 9, " %3i mph", F_07EE6(g_7eb0, 1) ? F_0E218(-1) : 0);
    sprintf(g_511e[1] + 5, " %2i gallons", g_7eb0->f22 / 10);
    switch (F_142CF(0x26, 1, ids, g_49e0, vals, g_511e)) {
    case -3:
        return;
    case 0x3ea:
        F_03AB8(0x0f4e);
        return;
    }
}

/* F_07AA4 @ 0000:7AA4 (linear 0x07AA4, 1090 bytes) */


static void near F_07AA4(void)
{
    int n;
    int flags[2];
    char buf[142];
    int prev;
    struct Car *owner;
    int k;
    struct Node *tbl[142];

    owner = g_7eb0;
    if (!owner) {
        F_03AB8(0x0efa);
        return;
    }
    for (k = 0; k < 2; k++) {
        if (prev = owner->b14[k])
            flags[k] = prev == 1 ? 1 : 2;
        else
            flags[k] = 0;
        g_1804[k].f0 = 11;
    }
    g_8286 = owner->slot6 ? g_49d2[g_4808[owner->slot6->idx].kind] : 0;
    g_vid.f16 = !g_8286;
    F_119B8(0x42e, 0x20, 10, -1);
    g_vid.f16 = 1;
    F_127DB(g_8286, owner->b14);
    F_13DE6(20);
    for (;;) {
        while ((k = F_01417(3000)) == 0)
            ;
        switch (k) {
        case -1:
            F_13F21(1);
            return;
        case -2:
            F_076A7(1, tbl, buf, 0);
            for (;;) {
                switch (k = F_142CF(4, 1, 0, g_49e0, buf, 0)) {
                case -3:
                    if (owner->slot6) {
                        F_03AB8(0x1875);
                        k = 0;
                    } else
                        k = F_14C8B();
                    break;
                case -4:
                    k = 0;
                    break;
                case 0x3ea:
                    F_03AB8(0xf72);
                    k = 0;
                    break;
                }
                if (!k)
                    break;
                prev = g_8286;
                g_8286 = g_49d2[g_4808[tbl[k]->idx].kind];
                if (g_7d88[owner->f2].f2 & g_4808[tbl[k]->idx].f1 & 0xf) {
                    F_07741(F_077B7(tbl[k]), &owner->slot6);
                    F_120FE(-4);
                    F_16A5A(1);
                    F_1264E(g_8286, 0);
                    F_16A5A(0);
                    F_16B97();
                    g_05fc += 0x666L;
                    for (k = 2; k--;) {
                        flags[k] = 1;
                        F_123F4(owner->b14[k] = 1, k);
                        F_16B61();
                        g_1804[k].f0 = 11;
                    }
                    F_00D78();
                    F_120FE(-2);
                    break;
                }
                    F_1264E(g_8286, 1);
                    F_03AB8(0x10cd);
                    F_12784(g_8286, 1);
                    g_05fc += 0x888L;
                    g_8286 = prev;
            }
            break;
        case 1:
        case 2:
            if (g_8286 == 0)
                break;
            if (flags[k-- - 1] == 2) {
                if (--owner->b14[k] <= 1) {
                    flags[k] = owner->b14[k] = 0;
                    F_12525(k);
                    F_16B61();
                    F_00D78();
                } else {
                    F_123F4(owner->b14[k], k);
                    F_16CF3();
                }
                if (flags[0] || flags[1])
                    break;
                F_12784(g_8286, 0);
                g_05fc += 0x666L;
                g_8286 = 0;
                owner->b14[0] = owner->b14[1] = 0;
                F_07789(&owner->slot6);
                F_00D78();
            } else if (flags[k] == 1) {
                if (owner->b14[k] == 3) {
                    flags[k] = 2;
                    break;
                }
                F_123F4(++owner->b14[k], k);
                F_16CF3();
            } else {
                F_120FE(-4);
                F_123F4(owner->b14[k] = 1, k);
                F_16B61();
                F_120FE(-2);
                flags[k] = 1;
            }
            break;
        }
    }
}

/* F_07EE6 @ 0000:7EE6 (linear 0x07EE6, 203 bytes) */


int F_07EE6(struct Car *car, int flag)
{
    int n, i;

    if (car->rack.s[0] == 0 || car->f1f <= 0 || car->rack.s[1] == 0)
        return 0;
    n = g_5126[g_4806[car->rack.s[1]->idx].f2];
    for (i = 2; i < n + 2; i++)
        if (car->rack.s[i] == 0)
            return 0;
    for (i = n * 2 + 3; --i >= 0; )
        if (car->b16[i] != 3)
            return 0;
    if (flag == 0)
        return 1;
    if (car->slot6 == 0)
        return 0;
    if (car->slot12 == 0)
        return 0;
    for (i = 2; --i >= 0; )
        if (car->b14[i] != 3)
            return 0;
    return 1;
}

/* F_07FB1 @ 0000:7FB1 (linear 0x07FB1, 506 bytes) */


#pragma intrinsic(inp, outp, abs)

static void near F_07FB1(struct Car *car)
{
    struct Obj *p;
    long tmp;
    struct Blt blt;
    union Word w;
    int key;
    int y;
    unsigned n;
    int gear;

    F_13DE6(0x1c);
    blt.w = 8;
    blt.h = 11;
    blt.sx = 0x30;
    blt.sy = 0;
    blt.dx = 0xb0;
    F_119B8(0x42f, 0x60, 9, -1);
    p = g_7568 + F_11149(8);
    tmp = g_0600;
    F_00D99();
    g_58d4 = 2;
    if (g_47ce) {
        outp(0x61, inp(0x61) | 3);
        g_47cf = 1;
    }
    do {
        if (g_47ce && g_47ce != g_47cf) {
            outp(0x61, inp(0x61) | 3);
            g_47cf = 1;
        }
        gear = car->f20;
        w.w = (abs(gear) << 10) + 30000;
        outp(0x42, w.b[0]);
        outp(0x42, w.b[1]);
        y = blt.dy = gear * 2 + 0x4b;
        F_120FE(-4);
        (*g_78a2)(p->img, g_vid.back, &blt, 1);
        F_1A92A(g_vid.back, g_vid.front, 0xb0, y, 0xb7, y + 10);
        F_120FE(-2);
        while ((key = F_01417(0)) == 0) {
            if (g_47ce && g_47ce != g_47cf) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
            }
        }
        switch (key) {
        case -10:
            car->f20++;
            if (car->f20 > 4)
                car->f20 = 4;
            break;
        case -7:
            car->f20--;
            if (car->f20 < -8)
                car->f20 = -8;
            break;
        }
    } while (key != -1);
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    g_58d4 = 1;
    F_00DD8();
    F_10D6C(8);
    n = ((unsigned)g_0600 - (unsigned)tmp) * 3;
    n = n > 0x444 ? 0x444 : n;
    g_05fc += n;
    F_13F21(1);
}
