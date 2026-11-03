/* F_15C4A: try the pictures of the zero terminated list in turn with F_1563A (load flag 0) and
   count the ones that succeed. A failure ends the walk unless all is set. Returns the count */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1563A(int n, int load, long dst);

int F_15C4A(int *list, int all)
{
    int n;
    int count;

    count = 0;
    while (n = *list++) {
        if (!F_1563A(n, 0, 0L)) {
            if (!all)
                return count;
        } else
            count++;
    }
    return count;
}
