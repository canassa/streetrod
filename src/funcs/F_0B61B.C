/* F_0B61B @ 0000:B61B (linear 0x0B61B, 252 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0B61B(struct Dlg *d, int id)
{
    struct Bitmap far *back, far *label, far *work;
    struct Blt box;

    if (d->f14)
        back = d->bm1;
    else
        back = d->bm2;
    F_120FE(-3);
    label = F_15BBE(id, 2);
    F_120FE(-1);
    work = F_1934E(back->width, back->height, 0, 2);
    F_19ED0(back, work, 1);
    box.w = label->width;
    box.h = box.fc = label->height;
    box.sx = box.sy = 0;
    box.dx = (~g_vid.mask & g_826e) - box.w + 24;
    box.dy = 12 - label->height;
    F_1A2D0(label, work, &box, 0);
    F_11BD3(work, g_826e - 16, g_8270 - 5);
    F_19438(2);
}
