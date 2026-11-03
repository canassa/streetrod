/* F_16564: siren: reps times sweep the tone from centre - width up to centre + width and back down
   again, in steps of step, each tone held for dur */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164CC(long freq, int delay);

static void near F_16564(int centre, int width, int step, int dur, int reps)
{
    int j;
    int k;

    for (k = 0; k < reps; k++) {
        for (j = 0; j * step < width * 2; j++)
            F_164CC((long)(centre - width + j * step), dur);
        for (j = 0; j * step < width * 2; j++)
            F_164CC((long)(centre + width - j * step), dur);
    }
}
