#include "STRUCTS.H"
#include "M_1923C.H"
#include "M_00000.H"
#include "M_036A8.H"

static char far *g_6cd0;                   /* DS:6CD0 */

/* F_192AC: allocate size bytes from the arena at g_6cd0; mode picks which of four marker stacks is used */


char far *F_192AC(int size, int mode)
{
    char far *p;

    p = g_6cd0;
    switch (mode) {
    case 0:
        p += o_8f00;
        o_8f00 += size;
        c_8b02[0] = o_8f00;
        break;
    case 1:
        p += c_8b02[g7b14o];
        c_8b02[g7b14o + 1] = size + c_8b02[g7b14o];
        g7b14o++;
        break;
    case 2:
        c_8b02[g7d80q] -= size;
        p += c_8b02[g7d80q];
        c_8b02[g7d80q - 1] = c_8b02[g7d80q] - 1;
        g7d80q--;
        break;
    case 3:
        bl78de -= size;
        p += bl78de--;
        c_8b02[60] = bl78de;
        break;
    }
    return p;
}

/* F_1934E: allocate a bitmap w x h from the arena (header plus pixel rows) and initialise it */


struct Bitmap far *F_1934E(int w, int h, int flags, int mode)
{
    int cols, extra, total;
    unsigned size;
    struct Bitmap far *mem;
    struct Bitmap far *bm;

    cols = ((w - 1) >> r_vid.shift) + 1;
    size = cols * h;
    extra = 0;
    if (flags)
        extra = size;
    total = extra + 0x30;
    if (!(flags & 2))
        total += size * r_vid.planes;
    bm = mem = (struct Bitmap far *)F_192AC(total, mode);
    bm->bits.p = (char far *)mem + 0x30;
    F_0376B(w, h, flags, -1, bm);
    return bm;
}
