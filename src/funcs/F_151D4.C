/* F_151D4: show hot spot id and let the player edit the text s (at most len characters) in its box
   with the spot's colours; returns the editor's result when that is 0 or negative, else 0 */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_151D4(int id, int len, char *s)
{
    int rl;

    F_13DE6(id);
    g_colors = g_style->pal[g_8ba2->f2];
    rl = F_14C90(g_8ba2->x1, g_8ba2->y1, s, len);
    F_13F21(1);
    return rl > 0 ? 0 : rl;
}
