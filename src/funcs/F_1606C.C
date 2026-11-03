/* F_1606C: clear the high bit of all 230 bytes of g_table again, unscrambling the hall of fame */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_1606C(void)
{
    int i;

    for (i = 0; i < 230; i++)
        ((char *)g_table)[i] &= 0x7f;
}
