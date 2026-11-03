#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_10E3D(int n);

int F_11149(int n)
{
    int i;

    for (i = 0; i < 5; i++) {
        if (g_7568[i].id == -1) {
            g_58f0[g_59f8[n]].a = i;
            g_7568[i].id = n;
            g_7568[i].f2 = g_59f8[n] + 1;
            g_7568[i].f4 = 0;
            F_10E3D(i);
            break;
        }
    }
    return i;
}
