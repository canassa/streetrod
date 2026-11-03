/* F_15127: number field of the current hot spot g_8ba2: edit a text into buf (at most 10 characters,
   fewer when the box is narrow: 6 pixels each) with the spot's colours and store its value in *out.
   Returns the editor's result when that is 0 or negative, else 0 */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

int F_15127(int *out, char *buf)
{
    int rl;

    g_colors = g_style->pal[g_8ba2->f2];
    g_09f2 = 1;
    rl = F_14C90(g_8ba2->x1, g_8ba2->y1, buf,
        (g_8ba2->x2 - g_8ba2->x1) / 6 > 10 ? 10 : (g_8ba2->x2 - g_8ba2->x1) / 6);
    g_09f2 = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}
