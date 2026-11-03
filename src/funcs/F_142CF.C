/* F_142CF: run the F_1438E dialog over a grabbed screen area (see F_14241) and restore the area
   afterwards; 0x3ea when *list is zero */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_142CF(int a, int b, int *c, int *list, char *buf, char **d)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    if (*list == 0)
        return 0x3ea;
    bm1 = F_14090(a, &x, &y1, 2);
    res = F_1438E(a, b, c, list, buf, d);
    F_11BD3(bm1, x, y1);
    F_19438(1);
    return res;
}
