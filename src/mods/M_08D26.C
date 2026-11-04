#include "STRUCTS.H"
#include "M_08D26.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_06476.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11E9C.H"
#include "M_163BE.H"
#include "M_16EA2.H"
#include "M_174A6.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (CLAUDE.md 4e). */
int jz79a6;

char g_51ac = -1;   /* DS:51AC */
char g_51ad = -1;   /* DS:51AD */

/* F_08D26 @ 0000:8D26 (linear 0x08D26, 118 bytes) */

void F_08D26(void)
{
    g_47ee = g_05ee = g_471c = g_4732 = 0;
    if (g_0284) {
        if (a_7eaap.m7eb0->f22 > 2)
            a_7eaap.m7eb0->f22 -= 2;
        else
            a_7eaap.m7eb0->f22 = 0;
        (*bl78be)();
        if (g_47ee == 0) {
            F_120FE(-4);
            g_58e6 = !g_4806[a_7eaap.m7eb0->slot6->idx].f2;
            F_0CDAF(&e_78e0);
            jz79a6 = 0;
            bw8aca = 0;
            F_08E2D();
        }
    }
}

/* F_08D9C @ 0000:8D9C (linear 0x08D9C, 145 bytes) */

void F_08D9C(int mode)
{
    char a, b;

    if (r_vid.mode == -2 || r_vid.mode == -6) {
        if (mode == -1) {
            a = g_51ac;
            b = g_51ad;
            g_51ac = g_51ad = -1;
        } else {
            if (g_51ac == -1) {
                g_51ac = (*(char *)&g_palette[12]);
                g_51ad = (*(char *)&g_palette[3]);
            }
            a = g_50c2[mode][0];
            b = g_50c2[mode][1];
        }
        if (gz8249) {
            F_112CB(12, a);
            F_112CB(3, b);
        }
        (*(char *)&g_palette[12]) = a;
        (*(char *)&g_palette[3]) = b;
    }
}

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
