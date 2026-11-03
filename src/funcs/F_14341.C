/* F_14341: wear text for a part: "(new)" when wear is 0, else "(xx% worn)" built in place in g_07aa
   (initialised data "(xx% worn)") with the number written over the xx */

#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static char * near F_14341(int wear)
{
    char *p;

    if (wear == 0)
        return "(`new`)";
    g_07aa[3] = ' ';
    itoa(wear, p = g_07aa + 1, 10);
    p[strlen(p)] = '%';
    return g_07aa;
}
