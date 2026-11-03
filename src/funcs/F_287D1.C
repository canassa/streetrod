/* F_287D1 @ 2645:2381 (linear 0x287D1, 111 bytes)

   Draws a clipped vertical line (color, x, y1, y2). */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_287D1(int color, int x, int y1, int y2)
{
    if (y2 >= g_clip_ymin && y1 <= g_clip_ymax && x >= g_clip_xmin && x <= g_clip_xmax) {
        y1 = (y1 < g_clip_ymin) ? g_clip_ymin : y1;
        y1 = (y1 > g_clip_ymax) ? g_clip_ymax : y1;
        y2 = (y2 < g_clip_ymin) ? g_clip_ymin : y2;
        y2 = (y2 > g_clip_ymax) ? g_clip_ymax : y2;
        (*g_78d2)(x, y1, x, y2, color);
    }
}
