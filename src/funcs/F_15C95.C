/* F_15C95: free the pictures listed in the zero terminated array of picture numbers */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_15C95(int *list)
{
    int n;

    while (n = *list++)
        F_158DE(n);
}
