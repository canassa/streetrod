/* F_08E2D @ 0000:8E2D (linear 0x08E2D, 123 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_08E2D(void)
{
    int r, saved;

    if (g_8aca)
        (*g_78be)();
    g_8228 = g_7d66 = 0;
    saved = g_827c;
    F_067CF(g_7eb0->f4);
    if (g_8aca)
        F_08D9C(saved);
    r = F_16EA2();
    F_16AC1(0);
    F_16AA8(0);
    g_472a = g_4746;
    g_472c = g_4748;
    g_4746 = 0;
    g_4748 = 0;
    return r;
}
