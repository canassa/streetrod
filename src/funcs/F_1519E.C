/* F_1519E: show hot spot id, read a number into *out with F_15127 and close the spot again;
   returns the editor's result */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_1519E(int id, int *out)
{
    int res;
    char buf[12];

    F_13DE6(id);
    buf[0] = 0;
    res = F_15127(out, buf);
    F_13F21(1);
    return res;
}
