/* F_28847 @ 2645:23F7 (linear 0x28847, 25 bytes)

   Sets the clip window to x 242..317, y 22..42. */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_28847(void)
{
    g_clip_xmin = 0xf2;
    g_clip_ymin = 0x16;
    g_clip_xmax = 0x13d;
    g_clip_ymax = 0x2a;
}
