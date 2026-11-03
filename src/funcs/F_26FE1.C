/* F_26FE1 @ 2645:0B91 (linear 0x26FE1, 439 bytes)

   Builds the track profile arrays from the section list of the chosen layout. */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_265BA(int n);
static void near F_265CA(void);
static void near F_266E3(void);
static void near F_267F8(void);
static void near F_26989(void);
static void near F_26B1A(void);
static void near F_26BAD(void);
static void near F_26C42(void);
static void near F_26CD5(void);
static void near F_26D6A(void);
static void near F_26E40(void);
static void near F_26F0B(void);

void F_26FE1(int n)
{
    int i;

    g_6e00 = 0x186;
    g_6dfe = 0;
    g_6e02 = g_82c6 ? 2 : 3;
    for (i = 0x17c; i < 0x460; i++) {
        g_ter4570[i] = 0;
        g_ter56f0[i] = 0;
        g_ter4e30[i] &= 0xf117;
        g_ter3cb0[i] &= 0x87ff;
        g_ter33f0[i] &= 0xe7ff;
        if ((i + 1) % (g_82c6 * 4 + 4) == 0)
            g_ter4e30[i] |= 8;
    }
    if (n == 100)
        g_5e0c = F_265BA(10);
    for (i = 0; i < g_5cba; i++) {
        switch (g_5cbc[g_5e0c][i]) {
        case 1: F_265CA(); break;
        case 2: F_266E3(); break;
        case 3: F_26B1A(); break;
        case 4: F_26BAD(); break;
        case 5: F_26C42(); break;
        case 6: F_26CD5(); break;
        case 7: F_26D6A(); break;
        case 8: F_26F0B(); break;
        case 9: F_26E40(); break;
        case 10: F_267F8(); break;
        case 11: F_26989(); break;
        }
    }
    for (; g_6e00 < 0x460; g_6e00++) {
        g_ter4570[g_6e00] = g_6dfe;
        g_ter56f0[g_6e00] = 0;
    }
}
