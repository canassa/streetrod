/* F_158DE @ 0F38:65DE (linear 0x158DE, 80 bytes)

   Frees and clears the far pointer in slot n of the far table g_slots. */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_153D7(int n);

void F_158DE(int n)
{
    char far * far *pp;

    n = F_153D7(n);
    if (*(pp = &g_slots[n].ptr.p)) {
        F_036D6(*pp);
        *pp = 0;
    }
}
