/* F_265BA @ 2645:016A: near wrapper around random_int, used by the track builder */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_265BA(int n)
{
    return random_int(n);
}
