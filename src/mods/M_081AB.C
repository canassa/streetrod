#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_076A7(int t, struct Node **list4, char *buf, int start);
static void near F_07741(struct Node *n, struct Node **slot);
static void near F_07789(struct Node **slot);
static struct Node * near F_077B7(struct Node *n);
static void near F_07FB1(struct Car *car);

/* F_081AB @ 0000:81AB (linear 0x081AB, 2938 bytes): the garage screen: install, remove and tune parts */



static void near F_081AB(struct Car *car)
{
    struct Node *list4[142];
    struct Node *p6;
    int cnt1;
    int sel2;
    int key;
    int ok;
    int k;
    int tmp29;
    int step;
    char buf[142];
    int n;
    int havek[10];

    if (car == 0) {
        F_03AB8(0xefa);
        return;
    }
    for (key = 1; key <= 9; key++) {
        if (k = car->b16[key - 1]) {
            havek[key] = k == 1 ? 1 : 2;
            g_829a[key - 1] = g_49ca[k];
        } else {
            havek[key] = 0;
            g_829a[key - 1] = 0;
        }
    }
    g_82ac = car->f1f != 0 ? (car->f1f > 0 ? 0x447 : 0x446) : 0;
    if (p6 = car->rack.s[0])
        g_49a2 = g_4808[p6->idx].kind == 0 ? 0x438 : 0x439;
    for (key = 0; key < 5; key++)
        g_8290[key] = (p6 = car->rack.s[key]) ? g_4998[key][g_4808[p6->idx].kind] : 0;
    g_5148[0][0] = g_5148[0][1] = g_8290[0] == 0x434 ? 4 : 5;
    cnt1 = (g_8290[0] != 0) + (g_8290[1] != 0) + (g_8290[2] != 0) + (g_8290[3] != 0) + (g_8290[4] != 0);
    n = (p6 = car->rack.s[1]) ? g_5126[g_4808[p6->idx].kind] + 2 : 5;
    if (p6) {
        tmp29 = g_4808[p6->idx].kind;
        for (k = 3; k < 9; k++)
            g_5186[k] = g_5148[tmp29][k - 3];
    }
    F_120FE(-4);
    F_131B5(car);
    F_132AD(g_5186, g_519a, car == g_7eb0);
    F_120FE(-2);
    if (car != g_7eb0)
        return;
    F_1344A(g_5186, g_519a);
    F_13DE6(0x15);
    for (;;) {
        while ((key = F_01417(3000)) == 0)
            ;
        switch (key) {
        case -1:
            F_13F21(1);
            g_vid.f8 = 0;
            F_193FE();
            return;
        case 13:
            car->f1f = -car->f1f;
            if (car->f1f == 0)
                continue;
            if (car->f1f > 0) {
                g_82ac = 0x447;
                F_1309C();
                g_05fc += 0x6d;
                F_1344A(g_5186, g_519a);
                continue;
            }
            g_82ac = 0x446;
            F_1309C();
            g_05fc += 0x6d;
            g_82ae = 0;
            F_1344A(g_5186, g_519a);
            break;
        case -3:
            if (F_07EE6(car, 0)) {
                F_07FB1(car);
                F_120FE(-4);
                F_132AD(g_5186, g_519a, 1);
                F_120FE(-2);
            } else
                F_03AB8(0xea1);
            continue;
        case -2:
            F_076A7(g_513c[cnt1], list4, buf, 0);
        again:
            key = F_142CF(4, 1, 0, g_49e0, buf, 0);
            switch (key) {
            case -3:
                key = F_14C8B();
                break;
            case 0x3ea:
                if (g_513c[cnt1] == 0)
                    F_03AB8(0xf97);
                if (g_513c[cnt1] == 3)
                    F_03AB8(0xfd7);
                if (g_513c[cnt1] == 2)
                    F_03AB8(0xfb5);
                key = 0;
                break;
            case -4:
                key = 0;
                break;
            }
            if (key == 0)
                continue;
            if (cnt1 >= n) {
                F_03AB8(0x1875);
                continue;
            }
            sel2 = cnt1;
            switch (cnt1) {
            default:
                goto again;
            case 0:
                ok = g_4808[list4[key]->idx].f1 & g_7d88[car->f2].f2 & 0xf;
                g_82ae = 0;
                step = 0;
                car->f1f = -1;
                g_82ac = 0x446;
                g_05fc += ok ? 0x666 : 0x888;
                break;
            case 1:
                ok = (g_4808[k = list4[key]->idx].f1 & g_7d88[car->f2].f2 & 0xf) != 0
                     && g_4978[g_4808[car->rack.s[0]->idx].kind][tmp29 = g_4808[k].kind] != 0;
                g_82ae = 1;
                step = 1;
                n = g_5126[g_4808[list4[key]->idx].kind] + 2;
                for (k = 4; k < 10; k++)
                    g_5184[k] = g_5148[tmp29][k - 4];
                g_05fc += ok ? 0x333u : 0x444u;
                break;
            case 2:
            case 3:
            case 4:
                ok = (g_4808[k = list4[key]->idx].f1 & g_7d88[car->f2].f2 & 0xf) != 0
                     && g_4988[tmp29 = g_4808[car->rack.s[1]->idx].kind][g_4808[k].kind] != 0;
                g_05fc += ok ? 0x199u : 0x222u;
                step = 3;
                sel2 = 2;
                switch (g_82ae = g_5148[tmp29][0]) {
                case 4:
                case 5:
                case 6:
                    break;
                case 7:
                    if (car->rack.s[2] == 0)
                        break;
                    if (car->rack.s[3]) {
                        g_82ae = 9;
                        step += 4;
                        sel2 = 4;
                    } else {
                        g_82ae = 8;
                        step += 2;
                        sel2 = 3;
                    }
                    break;
                case 10:
                case 12:
                    if (car->rack.s[2] == 0)
                        break;
                    g_82ae++;
                    step += 2;
                    sel2++;
                    break;
                default:
                    goto again;
                }
            }
            g_8290[sel2] = k = g_4998[sel2][g_4808[list4[key]->idx].kind];
            if (ok) {
                F_07741(F_077B7(list4[key]), &car->rack.s[sel2]);
                F_120FE(-4);
                F_16A5A(1);
                F_12DD8(g_8290[sel2], 0);
                F_16A5A(0);
                F_16B97();
                if (sel2 == 0) {
                    g_49a2 = k == 0x434 ? 0x438 : 0x439;
                    g_5148[0][0] = g_5148[0][1] = k == 0x434 ? 4 : 5;
                    g_82ac = 0x446;
                    F_1309C();
                }
                for (k = g_5130[cnt1]; --k >= 0;) {
                    g_82b2 = k;
                    F_12A17(g_829a[step + k] = g_49ca[1]);
                    F_16B61();
                    havek[step + k + 1] = 1;
                    car->b16[step + k] = 1;
                }
                F_1344A(g_5186, g_519a);
                F_120FE(-2);
                cnt1++;
                car->f20 = random_int(13) - 8;
                continue;
            } else {
                F_16A5A(1);
                F_12DD8(g_8290[sel2], 1);
                F_16A5A(0);
                F_16B97();
                F_03AB8(0x10cd);
                F_16A5A(1);
                F_12F99(g_8290[sel2], 1);
                F_16A5A(0);
                g_8290[sel2] = 0;
                if (sel2 == 0) {
                    car->f1f = 0;
                    g_82ac = 0;
                }
                goto again;
            }
        default:
            if (key < 1 || key > 9)
                continue;
            k = tmp29 = 0;
            for (; k < 5; k++)
                if ((tmp29 += g_5130[k]) >= key)
                    break;
            if (g_8290[k] == 0)
                continue;
            g_05fc += 0x36;
            if (havek[key] == 2) {
                g_82ae = g_5184[key];
                g_82b2 = g_5198[key];
                if (--car->b16[key - 1] <= 1) {
                    g_829a[key - 1] = havek[key] = car->b16[key - 1] = 0;
                    F_12B7C();
                    F_16B61();
                } else {
                    F_12A17(g_829a[key - 1] = g_49ca[car->b16[key - 1]]);
                    F_16CF3();
                }
            done:
                sel2 = -1;
                ok = 0;
                switch (g_82ae) {
                default:
                    if (g_5198[k = key] == 1)
                        k--;
                    if (havek[k] != 0 || havek[k + 1] != 0)
                        goto strip;
                    sel2 = (k - 4) / 2 + 2;
                    if (cnt1 == 3)
                        ok = 2;
                    g_05fc += 0x199;
                    goto strip;
                case 0:
                    break;
                case 1:
                    if (cnt1 != 2)
                        goto strip;
                    for (key = 2; key < 4 && havek[key] == 0; key++)
                        ;
                    if (key != 4)
                        goto strip;
                    sel2 = 1;
                    ok = 1;
                    g_05fc += 0x333;
                    goto strip;
                }
            } else {
                if (havek[key] == 1) {
                    if (car->b16[key - 1] == 3) {
                        havek[key] = 2;
                        continue;
                    }
                    g_829a[key - 1] = g_49ca[++car->b16[key - 1]];
                    g_82ae = g_5184[key];
                    g_82b2 = g_5198[key];
                    F_12A17(g_829a[key - 1]);
                    F_16CF3();
                    continue;
                }
                g_82ae = g_5184[key];
                g_82b2 = g_5198[key];
                F_12A17(g_829a[key - 1] = g_49ca[1]);
                F_16B61();
                havek[key] = 1;
                car->b16[key - 1] = 1;
                continue;
            }
        }
        sel2 = -1;
        ok = 0;
        if (cnt1 == 1 && car->f1f == -1) {
            for (key = 1; key < 2 && !havek[key]; key++)
                continue;
            if (key == 2) {
                sel2 = 0;
                car->f1f = 0;
                g_82ac = 0;
                g_05fc += 0x666;
            }
        }
    strip:
        if (sel2 >= 0) {
            cnt1--;
            F_07789(&car->rack.s[sel2]);
            F_120FE(-4);
            g_8290[sel2] = 0;
            if (ok)
                g_82ae = ok - 1;
            F_131B5(car);
            F_132AD(g_5186, g_519a, 1);
            F_1344A(g_5186, g_519a);
            car->f20 = random_int(13) - 8;
            F_120FE(-2);
        }
        if (ok == 0)
            continue;
        goto done;
    }
}
