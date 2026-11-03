/* F_150B7: number prompt: draw the prompt s at (x, y) in the style's colours, let the player edit a
   text of up to 20 characters at (g_824c, g_824e) and store its value in *out. Returns the result
   of the editor when that is 0 or negative, else 0 */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

int F_150B7(int x, int y, char *s, int *out)
{
    char buf[20];
    int rl;

    g_colors = g_style->pal[5];
    F_119D6(x, y, s);
    g_09f2 = 1;
    buf[0] = 0;
    rl = F_14C90(g_824c, g_824e, buf, 20);
    g_09f2 = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}
