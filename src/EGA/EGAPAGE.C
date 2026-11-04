#include <conio.h>
#include "STRUCTS.H"
#include "EGAPAGE.H"
#include "PC.H"

/* egapage_show @ 21A0:182E (linear 0x2322E, 52 bytes)

   Set the CRTC start address and wait for a vertical retrace */

void egapage_show(int n)
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
