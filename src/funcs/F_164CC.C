/* F_164CC: play the tone of frequency freq for n ticks (F_21962) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164CC(long freq, int n)
{
    F_16455(freq);
    F_21962(n);
}
