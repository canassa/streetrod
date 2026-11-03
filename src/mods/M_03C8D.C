#include <stdio.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03C3E(struct Node *p7);

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
        for (p7 = g_7eb8, nl = 0; p7; p7 = p7->next) {
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
        *g_57a7 = 0x1f;
        sel = F_142CF(4, sel <= g_49e0[0] ? sel : g_49e0[0], 0, g_49e0, lbl, 0);
        *g_57a7 = 0x2e;
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
            g_7eb8 = p7->next;
        else
            list8[sel - 1]->next = p7->next;
        F_03C3E(p7);
        g_05fc += 0x444L;
        F_11540(price1);
    }
}
