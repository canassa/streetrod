#include "GLOBALS.H"
#include "PROTOS.H"
#include <dos.h>

static int near F_0018B(void);
static int near F_00082(int mode);

int g_0262 = 1;   /* DS:0262 */

/* F_00226 @ 0000:0226 (linear 0x00226, 240 bytes) */



static int near F_00226(int mode)
{
    int herc;
    int pick;

    herc = 0;
    F_0E62F();
    if (g_0252)
        g_0250 = 1;
    if (F_0E5C3() && (g_0250 == 1 || F_0E584())) {
        g_0250 = 0;
        herc = 1;
    }
    if (g_0250)
        mode = g_5af1 == 3 ? 2 : 6;
    else
        mode = 1;
    if (herc)
        mode = 4;
    if (F_24602("Tandy", (char far *)0xF0000000L, 0xffff) ||
        F_24602("TANDY", (char far *)0xF0000000L, 0xffff))
        mode = 5;
    if (F_0018B())
        mode = 3;
    pick = F_00082(mode);
    if (pick)
        mode = pick;
    if (mode == 1 && g_0250)
        return 6;
    if (mode == 3) {
        g_0254 = 1;
        return 2;
    }
    return mode;
}

/* F_00316 @ 0000:0316 (linear 0x00316, 478 bytes) */


static void near F_00316(int mode)
{
    switch (mode) {
    case 3:
        g_0254 = 1;
    case 2:
        r_vid.mode = -2;
        *(unsigned char far *)0x410L &= 0xcf;
        *(unsigned char far *)0x410L |= 0x20;
        F_22B28();
        return;
    case 5:
        r_vid.mode = g_screens[0].flags = 0xfa;
        F_22352();
        g_screens[0].bits.p = r_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
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
        r_vid.mode = -4;
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
    g_screens[0].bits.p = r_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
    g_screens[0].planesize = g_screens[1].planesize = 0x4000;
    g_screens[0].pitch = g_screens[1].pitch = 0x50;
    g_screens[1].flags = 0xfd;
    g_screens[1].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[1].bits.p, 0, 0x4000);
    g_screens[2].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[2].bits.p, 0, 0x4000);
}
