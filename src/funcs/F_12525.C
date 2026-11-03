/* F_12525: draw the bar for slot "slot" of the dialog, then (screen shadowed) grab it back and paste the frame */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_12525(int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    blt.sx = blt.sy = 0;
    blt.dx = x = 0xb8;
    blt.dy = y = g_06fe[slot] + 10;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    F_120FE(-1);
    if (g_vid.f16) {
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
        F_16CF3();
        F_120FE(-4);
        blt.sx = 0x40;
        tmp = F_1A89E(pict, &blt, 2);
        (*g_78ce)(0xe, 0x94, x, y, 0x94, tmp);
        F_19438(1);
        F_120FE(-2);
    }
    F_19438(1);
}
