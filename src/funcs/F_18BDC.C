/* F_18BDC: cut a sheet bitmap into equal cells, one sub-bitmap per cell */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_18BDC(struct Bitmap far **out, int n)
{
    int id5;
    struct Blt r;
    int cells;
    int cols8;
    struct Bitmap far *sheet;
    int i;

    id5 = g_5a26[n].id;
    cols8 = g_5a26[n].cols;
    cells = g_5a26[n].n;
    sheet = F_15BBE(id5, 2);
    r.w = sheet->width / cols8;
    r.h = sheet->height >> 1;
    r.sx = 0;
    r.sy = 0;
    i = 0;
    for (; i < cells; i++, r.sx += r.w) {
        if (i == cols8) {
            r.sx = 0;
            r.sy = r.h;
        }
        out[i] = F_1A89E(sheet, &r, 2);
    }
}
