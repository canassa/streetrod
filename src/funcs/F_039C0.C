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
