#include "GLOBALS.H"
#include "PROTOS.H"

/* F_0641E @ 0000:641E (linear 0x0641E, 87 bytes) */


static int near F_0641E(void)
{
    struct Bitmap far *p;
    int row;
    int x;
    int y;

    p = F_14090(0x1b, &x, &y, 2);
    row = F_14294(0x1b);
    F_11BD3(p, x, y);
    F_19438(1);
    return row;
}
