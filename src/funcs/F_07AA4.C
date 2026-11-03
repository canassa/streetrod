/* F_07AA4 @ 0000:7AA4 (linear 0x07AA4, 1090 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

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
