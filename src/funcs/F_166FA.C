/* F_166FA: fill buf with the values from start to end in steps of step (up or down); returns how many */
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_166FA(int *buf, int start, int end, int step)
{
    int n;
    int count;

    if (start < end) {
        for (n = 0; start <= end; n++)
            start = (buf[n] = start) + step;
    } else {
        for (n = 0; start >= end; n++)
            start = (buf[n] = start) - step;
    }
    count = n;
    return count;
}
