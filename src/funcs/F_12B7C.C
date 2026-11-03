/* F_12B7C: redraw the marker picture (0x43f) at the current slot position, aligned to the pixel grid.
   With the screen shadowed the area is also copied to the front buffer and a label is drawn beside it. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_12B7C(void)
{
    int ypos;
    int rows;
    int xpos;
    int y1;
    int wide;
    int x1;
    struct Blt blt;
    struct Bitmap far *pict;

    F_12964(&xpos, &ypos);
    x1 = xpos - 2;
    y1 = ypos;
    F_120FE(-4);
    pict = F_15BBE(0x43f, 2);
    wide = 8;
    if ((rows = xpos & 7) < 2) {
        xpos -= 8;
        wide = 0x10;
    } else if (rows > 4)
        wide = 0x10;
    xpos &= g_vid.mask;
    blt.w = pict->width;
    rows = blt.h = blt.fc = pict->height;
    blt.sx = x1;
    blt.dx = x1 += 0x60;
    y1 -= rows;
    blt.sy = y1;
    blt.dy = y1 += 9;
    (*g_78a6)(g_vid.f8, g_vid.back, &blt);
    if (g_vid.f16) {
        xpos += 0x60;
        ypos += 9 - rows;
        F_1A92A(g_vid.back, g_vid.front, xpos, ypos, xpos + wide, ypos + rows);
        F_16CF3();
        if (wide == 0x10)
            xpos += 8;
        (*g_78ce)(0xd, 0x93, xpos - 8, ypos + 3, 0x93, pict);
    }
    F_120FE(-2);
    F_19438(1);
}
