/* F_2874C @ 2645:22FC (linear 0x2874C, 53 bytes)

   Clips the vertical extent of a blit rectangle to the clip window. */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2874C(struct Blt *b)
{
    if (b->dy < g_clip_ymin) {
        b->h -= g_clip_ymin - b->dy;
        b->sy += g_clip_ymin - b->dy;
        b->dy = g_clip_ymin;
    }
    if (g_clip_ymax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clip_ymax;
}
