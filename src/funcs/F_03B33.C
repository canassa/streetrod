#include "GLOBALS.H"
#include "PROTOS.H"

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
