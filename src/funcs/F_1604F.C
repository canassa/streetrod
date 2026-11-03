/* F_1604F: set the high bit of all 230 bytes of g_table (10 hall of fame entries of 23 bytes) to scramble
   it before it is written to the file (see F_16152) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_1604F(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)g_table)[i] |= 0x80;
}
