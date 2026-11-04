#include <stdio.h>
#include <string.h>
#include "STRUCTS.H"
#include "M_039C0.H"
#include "HOTSPOT.H"
#include "MESSAGES.H"
#include "M_00000.H"
#include "M_008B4.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_02FC8.H"
#include "M_036A8.H"
#include "M_0542E.H"
#include "M_06476.H"
#include "M_08D26.H"
#include "M_0914C.H"
#include "M_0B3CA.H"
#include "M_0DCBE.H"
#include "M_0E56A.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_13682.H"
#include "M_139DE.H"
#include "M_14C90.H"
#include "M_15236.H"
#include "M_15396.H"
#include "M_1628A.H"
#include "M_163BE.H"
#include "M_174A6.H"
#include "M_1923C.H"
#include "M_193D4.H"
#include "M_19ED0.H"
#include "M_265BA.H"

static void near F_03C3E(struct Node *p7);
static int near F_04940(unsigned *price);
static int near F_039C0(void);
static int near F_03B33(void);
static void near F_0442B(int r);
static int near F_03B0C(void);
static void near F_03C8D(void);
static int near F_049ED(void);

struct Car *g_4f46 = 0;   /* DS:4F46 */
struct Car *g_4f48 = 0;   /* DS:4F48 */
int g_4f4a = 0;   /* DS:4F4A */
int g_4f4c = 0;   /* DS:4F4C */
int g_4f4e[5] = {1022, 1023, 1025, 1024, 1026};   /* DS:4F4E */
int g_4f58 = 0;   /* DS:4F58 */
int g_4f5a = -1;   /* DS:4F5A */

/* F_039C0 @ 0000:39C0 (linear 0x039C0, 195 bytes) */

static int near F_039C0(void)
{
    F_1013D(0);
    F_01A05(2, 10);
    F_0FF70();
    (*bl78be)();
    F_1013D(1);
    random_int(-1);
    (*bl78be)();
    if (by70e2 == 0)
        F_1552B(2);
    if (ag8bcet == 0) {
        if (protect_check() == 0)
            F_00FEA();
        F_120FE(-4);
    }
    F_27198();
    atnewName[0] = 0;
    g_0610 = ag8bcet;
    if (by70e2 == 0) {
        F_15EF9();
        if (r_vid.mode == -2)
            F_15C07(g_4c28);
    }
    F_15C4A(g_4c28, 1);
    F_120FE(-2);
    return -1;
}

/* F_03A83 @ 0000:3A83 (linear 0x03A83, 53 bytes) */

void F_03A83(int a)
{
    g_05cc = a + g_tick;
    g_05d0 = 1;
    F_01417(30000);
    g_05cc = 0;
    g_05d0 = 0;
}

/* F_03AB8 @ 0000:3AB8 (linear 0x03AB8, 84 bytes) */

void F_03AB8(int n)
{
    long saved;

    saved = g_05cc;
    F_13DE6(24);
    F_11C24(n);
    F_03A83(g_05dc);
    F_11E5F();
    F_13F21(1);
    g_05cc = saved;
}

/* F_03B0C @ 0000:3B0C (linear 0x03B0C, 39 bytes) */

static int near F_03B0C(void)
{
    if (a_7eaap.m7eb6 > 15) {
        F_03AB8(0x0e27);
        return 1;
    }
    if (a_7eaap.m7ebc > 130) {
        F_03AB8(0x0e3f);
        return 1;
    }
    return 0;
}

/* F_03B33 @ 0000:3B33 (linear 0x03B33, 232 bytes) */

static int near F_03B33(void)
{
    int pick5;
    int tune;
    int old;

    old = r_vid.f16;
    r_vid.f16 = 0;
    F_118D4(0x3fd, -1);
    if ((tune = F_06476()) != g_4f5a) {
        if ((pick5 = random_int(5)) == g_4f58)
            pick5 = random_int(5);
        g_4f58 = pick5;
        g_4f5a = tune;
    }
    F_119B8(g_4f4e[g_4f58], 0, 0x88, -1);
    F_120FE(-3);
    F_06750(0x79);
    F_120FE(-1);
    r_vid.f16 = old;
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xbd);
    F_120FE(1);
    return F_14294(7);
}

/* F_03C1B @ 0000:3C1B (linear 0x03C1B, 35 bytes) */

struct Node * near F_03C1B(void)
{
    struct Node *p;

    p = a_7eaap.m7eba;
    a_7eaap.m7eba = p->next;
    p->next = 0;
    a_7eaap.m7ebc++;
    return p;
}

/* F_03C3E @ 0000:3C3E (linear 0x03C3E, 28 bytes) */

static void near F_03C3E(struct Node *p)
{
    p->next = a_7eaap.m7eba;
    p->idx = -1;
    a_7eaap.m7eba = p;
    a_7eaap.m7ebc--;
}

/* F_03C5A @ 0000:3C5A (linear 0x03C5A, 51 bytes) */

int F_03C5A(struct Node *n)
{
    int r;

    if (g_4806[n->idx].f4 == 0 && g_4806[n->idx].f2 == 0)
        r = 100;
    else
        r = n->a;
    return r;
}

/* The file is compiled /Oalsi, but the loops of F_03C8D are plain /Ol-off
   loops in the shipped code (list counters and pointers kept in memory, test
   reached by a jmp). No for/while/do formulation reproduces that under /Ol,
   so loop optimization is switched off for this function only. */
#pragma loop_opt(off)

/* F_03C8D @ 0000:3C8D (linear 0x03C8D, 804 bytes) */

static void near F_03C8D(void)
{
    unsigned price1;
    struct Node *list8[142];
    unsigned sel;
    struct Node *p7;
    unsigned pct;
    struct Item *ip;
    char lbl[142];
    int type8;
    int tries3;
    int nl;
    int msg5;
    int cut;
    int rl;

    tries3 = 0;
    sel = 1;
    for (;; tries3++) {
        for (p7 = a_7eaap.m7eb8, nl = 0; p7; p7 = p7->next) {
            nl++;
            g_49e0[nl] = (ip = &g_4806[(list8[nl] = p7)->idx])->f6;
            if (p7->b != -0x80) {
                lbl[nl] = p7->b / 100;
                continue;
            }
            lbl[nl] = 0x80;
        }
        g_49e0[0] = nl;
        strcpy(g_2a3e, "Sell it");
        *(*(char * *)&g_579f[4]) = 0x1f;
        sel = F_142CF(4, sel <= g_49e0[0] ? sel : g_49e0[0], 0, g_49e0, lbl, 0);
        *(*(char * *)&g_579f[4]) = 0x2e;
        strcpy(g_2a3e, "Change");
        switch ((int)sel) {
        case -4:
            return;
        case 0x3ea:
            F_03AB8(tries3 ? 0x0ed7 : 0x0f2a);
            return;
        case -3:
            sel = F_14C8B();
            break;
        }
        p7 = list8[sel];
        price1 = F_03C5A(p7);
        type8 = p7->idx;
        pct = (10000 - p7->b) / 100;
        switch (g_4806[type8].f4) {
        case 2:
        case 3:
            rl = random_int(20);
            price1 = (float)price1 * ((rl + 70) / 100.0);
            break;
        case 0:
            ;
        case 1:
            cut = price1 * (pct / 100.0) * 0.9;
            price1 -= cut;
            price1 = (unsigned)(price1 * (pct / 100.0) * 0.9) + cut;
            break;
        case 4:
            price1 = 0;
            break;
        }
        price1 = price1 / 5 * 5;
        strcpy(g_2a5f, &g_2c02[g_4806[type8].f6]);
        if (price1) {
            msg5 = 0x100a;
            sprintf(g_3c0c, "Will you take $%-d ?", price1);
        } else
            msg5 = 0x1021;
        F_11DE2(msg5, -1, -1);
        nl = F_14241(16);
        F_11E5F();
        if (nl == -0x15)
            continue;
        if (sel == 1)
            a_7eaap.m7eb8 = p7->next;
        else
            list8[sel - 1]->next = p7->next;
        F_03C3E(p7);
        g_05fc += 0x444L;
        F_11540(price1);
    }
}

#pragma loop_opt(on)

/* F_03FB1 @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */

struct Node2 * near F_03FB1(void)
{
    struct Node2 *p;

    p = a_7eaap.m7eb4;
    a_7eaap.m7eb4 = p->next;
    p->next = 0;
    a_7eaap.m7eb6++;
    return p;
}

/* F_03FD4 @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */

static void near F_03FD4(struct Node2 *p)
{
    p->next = a_7eaap.m7eb4;
    p->b = -1;
    a_7eaap.m7eb4 = p;
    a_7eaap.m7eb6--;
}

/* F_03FF0 @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */

struct Node * near F_03FF0(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = F_03C1B();
    n->idx = idx;
    it = &g_4806[idx];
    n->b = g_4966[it->f4];
    n->a = a;
    n->next = a_7eaap.m7eb8;
    a_7eaap.m7eb8 = n;
    return n;
}

/* F_04036 @ 0000:4036 (linear 0x04036, 848 bytes) */

struct Car * near F_04036(int idx, int weight, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;

    car = (old == 0) ? (struct Car *)F_03FB1() : old;
    car->f2 = idx;
    car->weight = weight;
    car->f22 = 0x28;
    car->f5 = g7d86n[idx].f2;
    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    car->f4 = i;
    car->f1f = 1;
    car->f20 = random_int(13) - 8;
    car->slot6 = node = (old == 0) ? F_03C1B() : car->slot6;
    w = (g7d86n[idx].f4 >> 6) & 3;
    w = (kind = (g7d86n[idx].f4 & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[1];
    node->next = 0;
    car->rack.s[0] = node = (old == 0) ? F_03C1B() : car->rack.s[0];
    w = (g7d86n[idx].f4 >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[0];
    node->next = 0;
    car->slot12 = node = (old == 0) ? F_03C1B() : car->slot12;
    w = (g7d86n[idx].f4 >> 4 & 3) + 0x28;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[4];
    node->next = 0;
    car->rack.s[1] = node = (old == 0) ? F_03C1B() : car->rack.s[1];
    for (i = 3; i < 9; i++)
        car->b16[i] = 0;
    w = (i = g7d86n[idx].f4 >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[3];
    node->next = 0;
    w = (g7d86n[idx].f4 >> 14 & 3) + 0x16;
    i = g_5126[i] - 1;
    for (j = 2; j > i; j--)
        car->rack.s[j + 2] = 0;
    for (; i >= 0; i--) {
        car->rack.s[i + 2] = node = (old == 0) ? F_03C1B() : car->rack.s[i + 2];
        node->idx = w;
        node->a = g_4806[w].f0;
        node->b = g_4966[2];
        node->next = 0;
        car->b16[2 * i + 3] = 3;
        car->b16[2 * i + 4] = 3;
    }
    for (i = 0; i < 3; i++)
        car->b16[i] = 3;
    for (i = 0; i < 2; i++)
        car->b14[i] = 3;
    car->f24 &= 0xff00;
    car->f24 &= 0xe0ff;
    car->f24 &= 0xdfff;
    car->f24 &= 0xbfff;
    car->f24 &= 0x7fff;
    if (old == 0) {
        if (a_7eaap.m7eb0) {
            car->f26 = (struct Car *)a_7eaap.m7eb2;
            a_7eaap.m7eb2 = (int)car;
        } else
            a_7eaap.m7eb0 = car;
    }
    return car;
}

/* F_04386 @ 0000:4386 (linear 0x04386, 57 bytes) */

int near F_04386(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->a;
    if (keep) {
        n->next = a_7eaap.m7eb8;
        a_7eaap.m7eb8 = n;
    } else
        F_03C3E(n);
    return r;
}

/* F_043BF @ 0000:43BF (linear 0x043BF, 108 bytes) */

int near F_043BF(struct Car *c, int keep)
{
    register struct Node **p2;
    int r;
    register int i;

    c->weight -= F_04386(c->slot6, keep);
    r = 5;
    for (p2 = c->rack.s, i = r; i; p2++, i--)
        c->weight -= F_04386(*p2, keep);
    F_04386(c->slot12, 0);
    r = c->weight;
    if (r < 0)
        r = 0;
    F_03FD4((struct Node2 *)c);
    return r;
}

/* F_0442B @ 0000:442B (linear 0x0442B, 1301 bytes) */

static void near F_0442B(int tab)
{
    char list4[70];
    struct Bitmap far *bm1;
    char *p1;
    int off6;
    int used;
    struct Car *car;
    int sv;
    int x1;
    int y;
    int price6;
    struct Spec *specn;
    int n0;
    int ck;
    int i5;
    int k3;
    int n;
    char color;

    color = 0;
    g_0284 = 5;
    g_49e0[1] = 0xcd8;
    n = 2;
    i5 = price6 = 0;
    for (k3 = a_7d16q; k3 >= 0; k3 = g7d86n[k3].next) {
        g_49e0[n++] = g7d86n[k3].f6;
        list4[i5++] = k3;
    }
    g_49e0[n++] = 0xcfd;
    n0 = n;
    off6 = i5 - n0;
    for (k3 = 0; k3 < 0x2c; k3++) {
        if (g_4806[k3].f0 >= 0x14) {
            g_49e0[n++] = g_4806[k3].f6;
            list4[i5++] = k3;
        }
    }
    g_49e0[0] = n - 1;
    F_008B4(g_49e0, (tab == 1) ? 2 : 0x1c);
    goto test;
    do {
        used = n >= n0;
        if (used) {
            k3 = g_4806[i5 = list4[n + off6]].f6;
            price6 = g_4806[i5].f0;
            g_11b0 = 0xe;
        } else {
            k3 = (specn = &g7d86n[i5 = list4[n - 2]])->f6;
            price6 = specn->f0;
            g_11b0 = 10;
        }
        strcpy(g_2990, g_2c02 + k3);
        sprintf(g_29d6, "$%-3d ?", price6);
        color = -1;
        for (;;) {
            n = F_14294(9);
            if (n == -20) {
                if (g_0610 && (long)price6 > a_7eaap.m7eaa)
                    a_7eaap.m7eaa += 1999;
                if ((long)price6 > a_7eaap.m7eaa) {
                    p1 = strchr(g_3a5a, '$');
                    sprintf(++p1, "%d short.", price6 - (int)a_7eaap.m7eaa);
                    F_03AB8(0xf1a8);
                    break;
                }
                F_11540(-price6);
                if (used) {
                    F_03FF0(i5, price6);
                    g_05fc += 0x222;
                    if (a_7eaap.m7ebc > 0x82)
                        goto done;
                    break;
                }
                car = F_04036(i5, price6, 0);
                car->f4 = (color != -1) ? color : random_int(5);
                F_03AB8(0xf2de);
                g_05fc += 0x444;
                goto done;
            }
            if (n != -25)
                break;
            F_120FE(-4);
            F_193D4();
            bm1 = F_1934E(0x140, 0x52, 0, 0);
            F_11A72(0, 0x40, 0x140, 0x52, bm1);
            x_colors = bustyle->colors;
            ck = (r_vid.mode == -2 || r_vid.mode == -6) ? g_02c2[5] : g_02b4[5];
            F_1169A(0, 0x41, 0x140, 0x46, ck);
            F_1169A(0, 0x87, 0x140, 0xa, x_colors.c[1]);
            if (r_vid.mode != -2 && r_vid.mode != -6) {
                sv = x_colors.c[3];
                x_colors.c[3] = 3;
                F_1371C(r_vid.back, 0, 0x40, 0x52);
                F_1371C(r_vid.back, 0x13f, 0x40, 0x52);
                F_1371C(r_vid.front, 0, 0x40, 0x52);
                F_1371C(r_vid.front, 0x13f, 0x40, 0x52);
                F_11A24(0, 0x40, 0x140);
                F_11A24(0, 0x91, 0x140);
                x_colors.c[3] = sv;
            }
            if (color == -1) {
                color = random_int(5);
                F_067CF(color);
            }
            x1 = ((unsigned)specn - (unsigned)g7d86n) / 10;
            y = (((specn->f4 >> 8) & 7) == 2) ? 8 : 0;
            F_181C8(7, x1, y, 0, 0, 0x86, 0, g_5030[(specn->f4 >> 4) & 3]);
            F_119D6((0x140 - F_10B48(g_2990)) / 2, 0x88, g_2990);
            F_120FE(-2);
            g_05d0 = 1;
            F_14294(0x18);
            g_05d0 = 0;
            F_11BD3(bm1, 0, 0x40);
            F_193D4();
        }
test:
        n = F_009CB();
    } while (n != -3);
done:
    F_00D58();
    if (a_7eaap.m7eb0)
        F_067CF(a_7eaap.m7eb0->f4);
    g_0284 = 0;
}

char *g_4fa4 = g_3bf9;   /* DS:4FA4 */

/* F_04940 @ 0000:4940 (linear 0x04940, 98 bytes) */

static int near F_04940(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_4fa4, "How about $%-d ?", *price);
    F_11DE2(0xff7, -1, -1);
    key = F_14294(0xd);
    F_11E5F();
    return key;
}

/* F_049A2 @ 0000:49A2 (linear 0x049A2, 75 bytes) */

int F_049A2(struct Car *c)
{
    int r;

    r = 0;
    if (c->f24 & 0x2000)
        r |= 4;
    if (c->f24 & 0x4000)
        r |= 2;
    if (c->f24 & 0x8000)
        r |= 1;
    if (g_4806[c->rack.s[0]->idx].f2 == 2)
        r |= 8;
    return r;
}

/* F_049ED @ 0000:49ED (linear 0x049ED, 1501 bytes) */

static int near F_049ED(void)
{
    struct Bitmap far *pict;
    struct Car *tbl[142];
    unsigned offer;
    unsigned price;
    int rx;
    int mid;
    struct Car *cur;
    int ticks;
    int ye;
    int n;
    int result;
    unsigned cash;
    int item;

    pict = 0;
    ticks = 0;
    item = 1;
    for (;; ticks++) {
        n = 0;
        if (a_7eaap.m7eb0) {
            n = 1;
            g_49e0[1] = g7d86n[(tbl[1] = a_7eaap.m7eb0)->f2].f6;
        }
        for (cur = a_7eaap.m7eb2; cur; cur = cur->f26) {
            n++;
            g_49e0[n] = g7d86n[(tbl[n] = cur)->f2].f6;
        }
        g_49e0[0] = n;
        switch (item = F_142CF(0xb, (n > item) ? item : n, 0, g_49e0, 0, 0)) {
        case 0x3ea:
            F_03AB8(ticks ? 0xee9 : 0xefa);
        case -3:
            return 0;
        case -1:
            g_4f48 = tbl[item = F_14C8B()];
            g_4f46 = a_7eaap.m7eb0;
            if (g_4f48 != g_4f46) {
                if (item == ((a_7eaap.m7eb0 != 0) + 1))
                    a_7eaap.m7eb2 = g_4f48->f26;
                else
                    tbl[item - 1]->f26 = g_4f48->f26;
                if (a_7eaap.m7eb0) {
                    a_7eaap.m7eb0->f26 = a_7eaap.m7eb2;
                    a_7eaap.m7eb2 = a_7eaap.m7eb0;
                }
                a_7eaap.m7eb0 = g_4f48;
            } else
                g_4f48 = 0;
            return 3;
        case -2:
            break;
        default:
            if (item > 0)
                g_49e0[0] = g_49e0[0];
        }
        cur = tbl[item = F_14C8B()];
        offer = price = (int)((random_int(20) + 75.0) * cur->weight / 100.0);
        strcpy(g_2990, g_2c02 + g7d86n[cur->f2].f6);
        g_29f8[0] = 0;
        pict = F_14090(12, &rx, &ye, 1);
        F_13DE6(12);
        do {
            if ((n = F_15127(&cash, g_29f8)) == -22)
                goto tail;
        } while (cash == 0);
        if ((unsigned)(cur->weight * 1.15) < cash) {
            F_03AB8(0xec0);
            goto tail;
        }
        if (cur->weight < cash) {
            F_03AB8(0x106e);
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
            if (cur->weight < cash) {
                F_03AB8(0x1078);
                goto tail;
            }
        }
        if (cash > offer) {
            offer = (random_int(5) + 85.0) * price / 100.0;
            if (cash > offer && F_04940(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if (cash > offer) {
            offer = (random_int(5) + 91.0) * price / 100.0;
            if (cash > offer && F_04940(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if ((float)offer < (float)cash * 0.9) {
            F_03AB8(0x1084);
            goto tail;
        }
        F_03AB8((cur->weight > cash * 2) ? 0x104b : 0x1091);
        g_05fc += 0x888L;
        F_11540(cash);
        if (item == 1 && a_7eaap.m7eb0) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
            F_120FE(-4);
            mid = a_7eaap.m7eb0->f2;
            result = F_049A2(a_7eaap.m7eb0);
            F_181C8(6, mid, result, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                    g_5030[((struct Row8 *)((char *)g_4806 + 2))[a_7eaap.m7eb0->slot12->idx].kind]);
            F_13F21(1);
            F_06AA6();
            F_13DE6(10);
            F_120FE(-2);
            F_043BF(a_7eaap.m7eb0, 0);
            a_7eaap.m7eb0 = 0;
        } else {
            if (item == ((a_7eaap.m7eb0 != 0) + 1))
                a_7eaap.m7eb2 = cur->f26;
            else
                tbl[item - 1]->f26 = cur->f26;
            F_043BF(cur, 0);
        }
tail:
        F_13F21(1);
        if (pict) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
        }
    }
}

/* F_04FCA @ 0000:4FCA (linear 0x04FCA, 48 bytes) */

void F_04FCA(int h)
{
    F_11149(h);
    while (F_111F2(h))
        F_01417(10);
}

/* F_04FFA @ 0000:4FFA (linear 0x04FFA, 69 bytes) */

void F_04FFA(int a, int b, int c)
{
    long saved;

    saved = g_05cc;
    F_11DE2(a, b, c);
    F_03A83(g_05dc);
    F_11E5F();
    g_05cc = saved;
}

/* F_0503F @ 0000:503F (linear 0x0503F, 1007 bytes) */

void near F_0503F(void)
{
    int again;
    int kind;
    int v;
    int w;

    again = 1;
    kind = F_039C0();
    for (;;) {
        if (kind == -1)
            again = 1;
        w = kind;
        if (w == -1)
            kind = F_0570C();
        else
            kind = F_05B4A();
        if (kind == -666) {
            kind = -1;
            again = 1;
        }
        if (kind && again)
            continue;
        F_11593(g_49dc);
        v = 1;
        if (kind == 0 && w == -2 && a_7eaap.m7eb0)
            v = 2;
        if (kind == 0 && w == -2)
            v = 1;
        if (w == -1 && a_7eaap.m7eb6 <= 15 && a_7eaap.m7ebc <= 130) {
            kind = F_03B33();
            if (kind == 2 || kind == 1)
                F_0442B(kind);
        }
        again = 0;
        while ((kind = F_06B06(v)) != 0) {
            F_16381(0);
            if (kind == 0x29a) {
                if (g_0282 == 1)
                    F_100E2();
                F_0339F(g_0282 & 2);
                F_120FE(1);
                g_0282 = 0;
                again = 1;
                kind = F_0315B();
                if (kind != -2 && kind != -1 && kind == 4)
                    return;
                F_120FE(-2);
                g_4f4a = 0;
                break;
            }
            v = 2;
            switch (kind) {
            case 1:
                if (F_03B0C())
                    v = 0;
                else {
                    if (a_7eaap.m7eb0 == 0)
                        v = 1;
                    kind = F_03B33();
                    if (kind == 2 || kind == 1)
                        F_0442B(kind);
                    if (a_7eaap.m7eb0 == 0)
                        v = 2;
                    if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2)
                        v = 1;
                    g_05fc += 0x222;
                }
                continue;
            case 2:
            case 15:
                if (F_03B0C()) {
                    v = 0;
                    continue;
                }
                if (a_7eaap.m7eb0) {
                    if (F_07EE6(a_7eaap.m7eb0, 1)) {
                        F_0DCBE(a_7eaap.m7eb0, &e_78e0);
                        if (kind == 2) {
                            if (a_7eaap.m7eb0->f22 < 9) {
                                F_03AB8(0x172e);
                                v = 0;
                                continue;
                            }
                            g_0284 = 2;
                            F_08D26();
                            g_05fc += 0x222;
                            F_0AB8B();
                        } else {
                            if (a_7eaap.m7eb0->f22 >= 0x8c) {
                                F_03AB8(0x19b1);
                                v = 0;
                                continue;
                            }
                            if (a_7eaap.m7eaa == 0) {
                                F_03AB8(0x199f);
                                v = 0;
                                continue;
                            }
                            g_0284 = 3;
                            F_08D26();
                            g_05fc += 0x222;
                            F_0B8A1();
                        }
                        g_05fc += 0x222;
                        v = 1;
                    } else {
                        F_03AB8(0x0e7f);
                        v = 0;
                    }
                } else {
                    if (a_7eaap.m7eb2)
                        F_03AB8(0x0f16);
                    else
                        F_03AB8(0x0efa);
                    v = 0;
                }
                continue;
            case 4:
                v = 0;
                switch (kind = F_0641E()) {
                case -20:
                    return;
                case -22:
                case -4:
                    continue;
                case -3:
                    if (F_03B0C()) {
                        v = 0;
                        kind = 0x309;
                    } else {
                        F_05EEA();
                        v = 2;
                        if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2)
                            v = 1;
                        continue;
                    }
                case -2:
                case -1:
                default:
                    if (kind == 0x309)
                        continue;
                }
                break;
            case 5:
                F_03C8D();
                v = 0;
                continue;
            case 6:
                v = F_049ED();
                continue;
            case 7:
                F_06F6B();
                v = 0;
                continue;
            case 8:
                F_07800();
                v = 0;
                continue;
            case 9:
                F_07AA4();
                continue;
            case 10:
                F_193FE();
                F_081AB(a_7eaap.m7eb0);
                continue;
            case 13:
                F_0711A(4);
                v = 0;
                continue;
            case 11:
                F_0711A(2);
                v = 0;
                continue;
            case 12:
                F_0711A(1);
                v = 0;
                continue;
            case 14:
                F_07367();
                v = 0;
                continue;
            case 3:
                F_065AF(0);
                v = 0;
                continue;
            case 16:
                F_079EB();
                v = 0;
                continue;
            case 17:
                F_03AB8(0x18f6);
                v = 0;
                continue;
            case -40:
                if (g_58d0) {
                    F_00D99();
                    g_58d0 = 0;
                } else {
                    g_58d0 = 1;
                    F_00DD8();
                }
                v = 0;
                continue;
            }
            break;
        }
    }
}
