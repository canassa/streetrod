/* F_123F4: draw the 64 x 6 bar for slot of the dialog; frame selects the highlighted frame.
   When frame == 1 and the screen is shadowed, the old bar is grabbed and pasted first. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_123F4(int frame, int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    x = 0xb8;
    y = g_06fe[slot] + 10;
    if (frame == 1 && g_vid.f16) {
        blt.w = 0x10;
        blt.h = 6;
        blt.sx = 0x40;
        blt.sy = 0;
        tmp = F_1A89E(pict, &blt, 2);
        (*g_78ce)(0xe, 0x94, x, 8, y, tmp);
        F_19438(1);
    }
    blt.sx = frame << 4;
    blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
    F_19438(1);
}
