/* F_28860 @ 2645:2410 (linear 0x28860, 25 bytes)

   Sets the clip window to x 0..319, y 18..101. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_28860(void)
{
    g_clip_xmin = 0;
    g_clip_ymin = 0x12;
    g_clip_ymax = 0x65;
    g_clip_xmax = 0x13f;
}
