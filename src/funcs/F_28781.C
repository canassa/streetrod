/* F_28781 @ 2645:2331 (linear 0x28781, 80 bytes)

   Clips a blit rectangle to the clip window, vertically and then
   horizontally. */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_28781(struct Blt *b)
{
    if (b->dy < g_clip_ymin) {
        b->h -= g_clip_ymin - b->dy;
        b->sy += g_clip_ymin - b->dy;
        b->dy = g_clip_ymin;
    }
    if (g_clip_ymax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clip_ymax;
    if (b->dx < g_clip_xmin) {
        b->w -= g_clip_xmin - b->dx;
        b->sx += g_clip_xmin - b->dx + 7;
        b->dx = g_clip_xmin;
    }
}
