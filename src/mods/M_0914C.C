#include "STRUCTS.H"
#include "M_0914C.H"
#include "GBSS.H"
#include "M_0F38A.H"
#include "M_11E9C.H"
#include "M_15236.H"
#include "M_18BDC.H"

int g_51ee = 100;   /* DS:51EE */
int g_51f0 = 0;   /* DS:51F0 */

/* F_0914C @ 0000:914C (linear 0x0914C, 266 bytes) */


static int near F_0914C(int mode, int arg, int pct1, int pct2)
{
    int r;

    r = 0;
    switch (mode) {
    case 0:
        if (g_6c84 != 0)
            break;
        if (g_51f0 < 3 || random_int(100) >= pct1)
            break;
        r = 1;
        F_120FE(-4);
        F_18D59(0, arg);
        g_6c84 = 1;
        if (random_int(100) < pct2) {
            F_10AFD(100);
            F_18D59(1, arg);
            g_6c84 = 0;
            g_6c88 = 0;
        }
        break;
    case 1:
        if (g_6c84 != 1 || random_int(100) >= pct1)
            break;
        r = 1;
        F_120FE(-4);
        F_18D59(1, arg);
        g_6c84 = 0;
        g_6c88 = 0;
        g_51f0 = 0;
        break;
    }
    if (r)
        F_120FE(-2);
    return r;
}
