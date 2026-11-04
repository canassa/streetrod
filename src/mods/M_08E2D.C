#include "GLOBALS.H"
#include "PROTOS.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int g7d66n;

/* F_08E2D @ 0000:8E2D (linear 0x08E2D, 123 bytes) */


int F_08E2D(void)
{
    int r, saved;

    if (bw8aca)
        (*bl78be)();
    ao8228u = g7d66n = 0;
    saved = m_8272.m827c;
    F_067CF(a_7eaap.m7eb0->f4);
    if (bw8aca)
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
