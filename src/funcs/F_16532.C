/* F_16532 @ 0F38:71B2 (near): sweep a tone downwards from start while it is >= end, in steps of step */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164CC(long freq, int delay);

static void near F_16532(int start, int end, int step, int delay)
{
    int n;

    for (n = 0; start >= end; start -= step, n++)
        F_164CC((long)start, delay);
}
