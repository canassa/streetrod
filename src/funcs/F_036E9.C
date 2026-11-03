/* out_of_memory @ 0000:36E9 (linear 0x036E9, 32 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void out_of_memory(void)
{
    char *msg;

    msg = "Not enough memory to run Street Rod !";
    F_1174E(msg);
    F_00FEA();
}
