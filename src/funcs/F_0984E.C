/* F_0984E @ 0000:984E (linear 0x0984E, 108 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_09646(int);

static int near F_0984E(int chance, int arg)
{
    int pick;

    if (random_int(100) < chance)
        return 0x15;
    F_09646(arg);
    if (g_6c86 == 0)
        return 0x15;
    pick = g_6c8a[random_int(g_6c86)];
    if (pick == g_51f8)
        pick = g_6c8a[random_int(g_6c86)];
    g_51f8 = pick;
    return pick;
}
