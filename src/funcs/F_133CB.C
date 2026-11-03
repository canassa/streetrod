/* F_133CB: insert item into the array arr of n records, which is sorted by (f8, fa): it goes before
   the first record that compares greater, the rest are moved up by one. */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_133CB(struct Link *arr, struct Link *item, int n)
{
    struct Link *p;
    int i;

    p = arr;
    for (i = 0; i < n; i++, p++) {
        if (item->f8 < p->f8) {
            memmove(p + 1, p, (n - i) * sizeof(struct Link));
            *p = *item;
            return;
        }
        if (item->f8 <= p->f8 && item->fa < p->fa) {
            memmove(p + 1, p, (n - i) * sizeof(struct Link));
            *p = *item;
            return;
        }
    }
    *p = *item;
}
