#include "GLOBALS.H"
#include "PROTOS.H"

int F_0315B(void)
{
    char c01;
    char c1;
    char c21;
    char c3;
    int key;

    F_113A2(190);
    g_vid.f16 = 1;
    c01 = g_colors.c[0];
    c1 = g_colors.c[1];
    c3 = g_colors.c[3];
    c21 = g_8244;
    if (g_vid.mode != -2 && g_vid.mode != -6) {
        g_colors.c[0] = 3;
        g_colors.c[1] = 0;
        g_colors.c[3] = 2;
        g_8244 = 1;
    } else {
        g_colors.c[0] = 15;
        g_colors.c[1] = 1;
        g_colors.c[3] = 13;
        g_8244 = 3;
    }
    F_1169A(0, 0, 320, 190, g_colors.c[1]);
    F_13957(g_vid.back, 100, 50, 120, 90);
    F_13957(g_vid.front, 100, 50, 120, 90);
    F_13957(g_vid.back, 102, 52, 116, 86);
    F_13957(g_vid.front, 102, 52, 116, 86);
    g_colors.c[0] = c01;
    g_colors.c[1] = c1;
    g_colors.c[3] = c3;
    g_8244 = c21;
    F_120FE(-2);
    (*g_present)();
    key = F_14294(41);
    F_120FE(-4);
    switch (key) {
    case -2:
    case -1:
        if (g_4f4c) {
            F_15C4A(g_4c28, 1);
            g_4f4c = 0;
        }
        break;
    }
    return key;
}
