/* F_1783E: make a copy of a bitmap header that views the rows below y0, plus a bitmap to hold them (*pa), a second one (*pb), and two clip rects */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1783E(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb,
    int y1, int y0, struct Rect **pr1, struct Rect **pr2)
{
    struct Bitmap far *p2;
    struct Blt blt;
    int h, diff, flag, size, y2;
    struct Bitmap far *start;

    size = src->pitch;
    y2 = y0;
    h = y1 - y2;
    flag = mode != 0;
    start = (struct Bitmap far *)F_192AC(0x30, 1);
    *start = *src;
    start->bits.w.off += h * size;
    start->height = y2;
    start->planesize = y2 * size * g_vid.planes;
    if (g_vid.mode == -6) {
        if (g_5a22 == 0)
            g_5a22 = src->width;
        *pa = F_1934E(g_5a22, y2, flag, 1);
        blt.w = g_5a22;
        blt.h = y2;
        blt.sx = g_5a20;
        blt.sy = blt.dx = blt.dy = 0;
        (*g_78a2)(start, *pa, &blt, 1);
    } else {
        *pa = F_1934E(src->width, y2, flag, 1);
        F_19ED0(start, *pa, 1);
    }
    switch (g_vid.mode) {
    case -2:
        *pb = start;
        break;
    case -6:
        *pb = 0;
        break;
    default:
        *pb = F_1934E(start->width, y2, 0, 1);
        if (g_0284 == 6)
            F_19ED0(start, *pb, 1);
        break;
    }
    *pr1 = 0;
    *pr2 = 0;
    if (flag) {
        p2 = F_15BBE(mode, 2);
        if (g_vid.mode == -6) {
            blt.sy = p2->height - y2;
            (*g_78a2)(p2, *pa, &blt, 2);
        } else {
            diff = p2->planesize / g_vid.planes - (*pa)->planesize / g_vid.planes;
            F_1A882(p2->alt.p + diff, (*pa)->alt.p, (*pa)->planesize / g_vid.planes);
        }
        F_19438(1);
        *pr1 = &g_5a10;
        g_5a10.y2 = (*pa)->height - 1;
        *pr2 = &g_5a18;
        g_5a18.y1 = (*pa)->height - 15;
        if (0) ;
        (*pr2)->y2 = (*pa)->height - 1;
    }
}
