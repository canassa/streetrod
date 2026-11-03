/* data_segment @ 0000:36A8 (linear 0x036A8, 22 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

int data_segment(void)
{
    struct SREGS segs;

    segread(&segs);
    return segs.ds;
}
