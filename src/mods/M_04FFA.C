#include "STRUCTS.H"
#include "M_039C0.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_02FC8.H"
#include "M_06476.H"
#include "M_08D26.H"
#include "M_0914C.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_1628A.H"
#include "M_163BE.H"
#include "M_193D4.H"

static int near F_039C0(void);
static int near F_0570C(void);
static int near F_05B4A(void);
static int near F_03B33(void);
static void near F_0442B(int r);
static int near F_06B06(int v);
static int near F_03B0C(void);
static void near F_0DCBE(struct Car *c, struct Eng *e);
static int near F_0641E(void);
static void near F_05EEA(void);
static void near F_03C8D(void);
static int near F_049ED(void);
static void near F_06F6B(void);
static void near F_07800(void);
static void near F_07AA4(void);
static void near F_081AB(struct Car *c);
static void near F_0711A(unsigned mask);
static void near F_07367(void);

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



static void near F_0503F(void)
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
