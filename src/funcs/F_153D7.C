/* F_153D7: map a picture number to its table index: numbers from 1000 on are shifted down, the rest
   are 1 based. In the video modes -2 and -6 that is the result, else g_07c6 translates it */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_153D7(int n)
{
    if (n >= 1000)
        n -= 0x3df;
    else
        n--;
    if (g_vid.mode != -2 && g_vid.mode != -6)
        return g_07c6[n];
    else
        return n;
}
