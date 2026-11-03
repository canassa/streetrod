/* F_0570C @ 0000:570C (linear 0x0570C, 137 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_056A9(void);

static int near F_0570C(void)
{
    int sel;

    g_vid.f16 = 0;
    F_119B8(0x3f8, 0x10, 6, 0);
    g_vid.f16 = 1;
    F_10B9C();
    F_1A92A(g_vid.back, g_vid.front, 0, 0, 0x13f, 0xbd);
    (*g_present)();
    switch (sel = F_151D4(2, 15, g_newName)) {
    case -3:
    case 0:
        F_056A9();
        return 0;
    case -2:
        return -2;
    }
}
