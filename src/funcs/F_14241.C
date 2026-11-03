/* F_14241: grab the screen area around hot spot id, show the spot (waiting for a key through
   F_14294), put the saved area back and return the key */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_14241(int id)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    bm1 = F_14090(id, &x, &y1, 2);
    res = F_14294(id);
    F_11BD3(bm1, x, y1);
    F_19438(1);
    return res;
}
