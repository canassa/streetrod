/* F_15BBE: allocate a bitmap of the given mode for slot picture id, draw the picture into it
   (F_1592E) and return it */

#include "GLOBALS.H"
#include "PROTOS.H"

struct Bitmap far *F_15BBE(int id, int mode)
{
    int h;
    int w1;
    int x;
    struct Bitmap far *bm;

    F_15B7F(id, &w1, &h, &x);
    bm = F_1934E(w1, h, x, mode);
    F_1592E(id, bm);
    return bm;
}
