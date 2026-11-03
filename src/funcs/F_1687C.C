/* F_1687C: sweep noise bursts upwards from start while it is < end, in steps of step */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_167E4(int range, int bits, int count);

static void near F_1687C(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start + n * step < end; n++) {
        x = start + n * step;
        F_167E4(x, x - gap, dur);
    }
}
