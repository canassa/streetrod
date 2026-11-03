/* F_08D9C @ 0000:8D9C (linear 0x08D9C, 145 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

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
