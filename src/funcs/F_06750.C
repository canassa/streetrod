/* F_06750 @ 0000:6750 (linear 0x06750, 127 bytes)  - draws the current date ("Month day, 1963") */

#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0653C(int v, int *out);

void F_06750(int x)
{
    int date;
    int today;
    int mnum;
    char buf[30];
    struct Colors sv;

    today = F_06476();
    mnum = F_0653C(today, &date);
    sprintf(buf, "%s %d, 1963", g_5082[mnum], date);
    sv = g_colors;
    g_colors = g_style->hilite;
    (*g_78ae)(g_vid.back, 0x61, x, buf);
    g_colors = sv;
}
