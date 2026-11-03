#include "GLOBALS.H"
#include "PROTOS.H"

/* F_08D26 @ 0000:8D26 (linear 0x08D26, 118 bytes) */


void F_08D26(void)
{
    g_47ee = g_5ee = g_471c = g_4732 = 0;
    if (g_0284) {
        if (g_7eb0->f22 > 2)
            g_7eb0->f22 -= 2;
        else
            g_7eb0->f22 = 0;
        (*g_78be)();
        if (g_47ee == 0) {
            F_120FE(-4);
            g_58e6 = !g_4806[g_7eb0->slot6->idx].f2;
            F_0CDAF(&g_78e0);
            g_79a6 = 0;
            g_8aca = 0;
            F_08E2D();
        }
    }
}

/* F_08D9C @ 0000:8D9C (linear 0x08D9C, 145 bytes) */


void F_08D9C(int mode)
{
    char a, b;

    if (g_vid.mode == -2 || g_vid.mode == -6) {
        if (mode == -1) {
            a = g_51ac;
            b = g_51ad;
            g_51ac = g_51ad = -1;
        } else {
            if (g_51ac == -1) {
                g_51ac = g_044c;
                g_51ad = g_0443;
            }
            a = g_50c2[mode][0];
            b = g_50c2[mode][1];
        }
        if (g_8249) {
            F_112CB(12, a);
            F_112CB(3, b);
        }
        g_044c = a;
        g_0443 = b;
    }
}
