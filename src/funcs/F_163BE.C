/* F_163BE: calibrate the speed of the machine: counts how often F_21962(n) can be called within
   one BIOS timer tick (the long at 0040:006C). Counts above 848 are scaled by 3/4. Also sets
   g_58e2 to 256 on the way */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_163BE(int n)
{
    long tick;
    int count;

    count = 0;
    g_58e2 = 0x100;
    tick = *(long far *)0x46cL;
    while (tick == *(long far *)0x46cL)
        ;
    tick = *(long far *)0x46cL;
    while (tick == *(long far *)0x46cL) {
        F_21962(n);
        count++;
    }
    if (count > 0x350)
        count = 3 * count / 4;
    return count;
}
