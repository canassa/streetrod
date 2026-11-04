#include <conio.h>
#include "EGAPAGE.H"
#include "PC.H"

/* Set the CRTC start address and wait for a vertical retrace */

void ep_show(int n)
{
    outp(CRTC_INDEX, CRTC_START_HI);
    outp(CRTC_DATA, n << 5);
    outp(CRTC_INDEX, CRTC_START_LO);
    outp(CRTC_DATA, 0);
    while (inp(INPUT_STATUS) & STATUS_VRETRACE)
        ;
    while (!(inp(INPUT_STATUS) & STATUS_VRETRACE))
        ;
}
