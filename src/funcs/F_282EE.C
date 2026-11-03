/* F_282EE @ 2645:1E9E (linear 0x282EE, 94 bytes)

   Rebuilds the projection tables for both directions, then waits 9 ticks. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_282EE(void)
{
    long deadline;

    F_27B6D(g_7d30);
    F_27D4B();
    F_27B6D(g_7d30);
    F_2801C();
    F_27B6D(g_7d30);
    deadline = g_tick + 9;
    F_2AFDC();
    while (deadline > g_tick)
        ;
}
