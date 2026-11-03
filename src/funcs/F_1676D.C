/* F_1676D: warble: set the tone to freq, then reps times play the square wave half made by
   F_166FA (from 1 to hi in steps of step) forwards and backwards */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_16612(int a, int *p, int count, int reps);
static void near F_16686(int a, int *p, int count, int reps);
static int near F_166FA(int *buf, int start, int end, int step);

static void near F_1676D(long freq, int period, int hi, int step, int cycles, int reps)
{
    int count;
    int half[30];
    int k;

    count = F_166FA(half, 1, hi, step);
    F_16455(freq);
    for (k = 0; k < reps; k++) {
        F_16612(period, half, count, cycles);
        F_16686(period, &half[count - 1], count, cycles);
    }
}
