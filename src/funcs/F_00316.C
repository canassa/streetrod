/* F_00316 @ 0000:0316 (linear 0x00316, 478 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_00316(int mode)
{
    switch (mode) {
    case 3:
        g_0254 = 1;
    case 2:
        g_vid.mode = -2;
        *(unsigned char far *)0x410L &= 0xcf;
        *(unsigned char far *)0x410L |= 0x20;
        F_22B28();
        return;
    case 5:
        g_vid.mode = g_screens[0].flags = 0xfa;
        F_22352();
        g_screens[0].bits.p = g_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
        g_screens[0].planesize = g_screens[1].planesize = g_screens[2].planesize = 0x8000;
        g_screens[0].pitch = g_screens[1].pitch = g_screens[2].pitch = 0xa0;
        g_screens[1].flags = g_screens[2].flags = 0xfa;
        g_screens[1].bits.p = (char far *)0x90000000L;
        g_screens[2].bits.p = F_03709(0x8000, 1);
        F_109C0(g_screens[1].bits.p, 0, 0x8000);
        F_109C0(g_screens[2].bits.p, 0, 0x8000);
        return;
    case 4:
        g_screens[0].flags = 0xfc;
        g_vid.mode = -4;
        break;
    case 1:
        g_screens[0].flags = 0xfd;
        break;
    case 6:
        g_screens[0].flags = 0xfd;
        *(unsigned char far *)0x410L &= 0xcf;
        *(unsigned char far *)0x410L |= 0x20;
        break;
    }
    F_1C2B8();
    g_screens[0].bits.p = g_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
    g_screens[0].planesize = g_screens[1].planesize = 0x4000;
    g_screens[0].pitch = g_screens[1].pitch = 0x50;
    g_screens[1].flags = 0xfd;
    g_screens[1].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[1].bits.p, 0, 0x4000);
    g_screens[2].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[2].bits.p, 0, 0x4000);
}
