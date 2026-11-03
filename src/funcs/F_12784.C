/* F_12784: paste picture 0x42e at (0x20, 10) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_12784(int n, int flag)
{
    struct Bitmap far *p;

    F_120FE(-3);
    p = F_15BBE(0x42e, 2);
    F_120FE(-1);
    F_11BD3(p, 0x20, 10);
    F_19438(1);
}
