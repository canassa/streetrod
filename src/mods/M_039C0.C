#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_039C0(void)
{
    F_1013D(0);
    F_01A05(2, 10);
    F_0FF70();
    (*g_78be)();
    F_1013D(1);
    random_int(-1);
    (*g_78be)();
    if (g_70e2 == 0)
        F_1552B(2);
    if (g_8bce == 0) {
        if (protect_check() == 0)
            F_00FEA();
        F_120FE(-4);
    }
    F_27198();
    g_newName[0] = 0;
    g_0610 = g_8bce;
    if (g_70e2 == 0) {
        F_15EF9();
        if (g_vid.mode == -2)
            F_15C07(g_4c28);
    }
    F_15C4A(g_4c28, 1);
    F_120FE(-2);
    return -1;
}

void F_03A83(int a)
{
    g_05cc = a + g_tick;
    g_05d0 = 1;
    F_01417(30000);
    g_05cc = 0;
    g_05d0 = 0;
}

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

static int near F_03B0C(void)
{
    if (g_7eb6 > 15) {
        F_03AB8(0x0e27);
        return 1;
    }
    if (g_7ebc > 130) {
        F_03AB8(0x0e3f);
        return 1;
    }
    return 0;
}

static int near F_03B33(void)
{
    int pick5;
    int tune;
    int old;

    old = g_vid.f16;
    g_vid.f16 = 0;
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
    g_vid.f16 = old;
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, 0, 0, 0x13f, 0xbd);
    F_120FE(1);
    return F_14294(7);
}

/* F_03C1B @ 0000:3C1B (linear 0x03C1B, 35 bytes) */


static struct Node * near F_03C1B(void)
{
    struct Node *p;

    p = g_7eba;
    g_7eba = p->next;
    p->next = 0;
    g_7ebc++;
    return p;
}

/* F_03C3E @ 0000:3C3E (linear 0x03C3E, 28 bytes) */


static void near F_03C3E(struct Node *p)
{
    p->next = g_7eba;
    p->idx = -1;
    g_7eba = p;
    g_7ebc--;
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
