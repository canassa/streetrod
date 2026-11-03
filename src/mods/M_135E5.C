#include "GLOBALS.H"
#include "PROTOS.H"

/* F_135E5: sort n hot spots of g_09f4 starting at first by x1 (exchange sort, ascending) */


void F_135E5(int first, int n)
{
    struct Hot *p;
    struct Hot *q;
    int i;
    int k;
    struct Hot swap;

    for (i = 0; i < n - 1; i++) {
        p = &g_09f4[first + i];
        for (k = i + 1; k < n; k++) {
            q = &g_09f4[first + k];
            if (p->x1 > q->x1) {
                swap = *p;
                *p = *q;
                *q = swap;
            }
        }
    }
}
