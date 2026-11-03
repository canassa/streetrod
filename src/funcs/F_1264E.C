/* F_1264E: show picture n centered in the dialog area; flag moves it left by 24.
   With the screen shadowed the area under it is saved around the draw and restored afterwards. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1264E(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    struct Blt blt;
    int w1;
    int h1;
    int sx;
    int sy;

    F_120FE(-4);
    pict = F_15BBE(n, 2);
    sx = 200 - (w1 = pict->width);
    sy = 125 - (h1 = pict->height);
    if (flag)
        sx -= 0x18;
    if (g_vid.f16) {
        saved = F_11A72(sx - 8, sy, w1 + 8, h1, 0);
        (*g_78ce)(0xe, 0x94, sx - 8, 0x94, sy, pict);
        blt.sx = blt.sy = 0;
        blt.dx = 8;
        blt.dy = 0;
        blt.w = w1 + 8;
        blt.h = h1;
        (*g_78aa)(pict, saved, &blt, 1);
        F_120FE(-2);
        F_11BD3(saved, sx - 8, sy);
        F_19438(1);
    } else {
        (*g_78ce)(0xe, 0x94, sx, sy, sy, pict);
        F_120FE(-2);
    }
    F_19438(1);
}
