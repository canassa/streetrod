/* F_164E9: play count random tones with frequencies from lo up to hi, each held for dur ticks */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164CC(long freq, int n);

static void near F_164E9(int lo, int hi, int dur, int count)
{
    int span;
    int i;

    span = hi - lo;
    for (i = 0; i < count; i++)
        F_164CC((long)(random_int(span) + lo), dur);
}
