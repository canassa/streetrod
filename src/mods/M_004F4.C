#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

/* F_004F4 @ 0000:04F4 (linear 0x004F4, 210 bytes) */


static void near F_004F4(int mode)
{
    int far *p;

    switch (mode) {
    case 2:
    case 3:
        F_001D1(0xd);
        break;
    case 5:
        F_001D1(9);
        outp(0x3df, 0xf6);
        break;
    case 4:
        F_0E5D2();
        p = g_vid.mode == -4 ? (int far *)0xb0000000L : (int far *)0xb8000000L;
        while (p < (int far *)((g_vid.mode == -4 ? 0xb0000000L : 0xb8000000L) + 0x8000L))
            *p++ = 0;
        outp(0x3b8, 0x2a);
        g_0046 = 1;
        g_0042 = 1;
        break;
    case 1:
    case 6:
        F_001D1(4);
        F_001F2(1);
        break;
    }
    g_0048 = 1;
}
