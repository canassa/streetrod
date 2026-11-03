/* F_008B4 @ 0000:08B4 (linear 0x008B4, 259 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_008B4(int *list, int base)
{
    int num;
    int pos;
    int *ptr;
    int first;

    g_0536 = 0;
    F_193D4();
    g_pages.list = list;
    g_pages.n = 0;
    ptr = list + 1;
    pos = 0xbe;
    num = first = *list;
    base = num - base;
    g_pages.cur = 1;
    while (num--) {
        pos += 9 * g_2c01[*ptr] + 3;
        while (pos > 0xbd) {
            g_pages.start[g_pages.n++] = first - num;
            pos = 0;
            pos += 9 * g_2c01[*ptr] + 3;
        }
        if (num == base)
            g_pages.cur = g_pages.n - 1;
        ptr++;
    }
    g_colors = g_style->hilite;
    F_120FE(-3);
    g_0522 = 0x40;
    g_0524 = 0xbe;
    g_0526 = g_0528 = g_052a = g_052c = g_052e = 0;
    g_0520 = 1;
    F_13DE6(8);
    F_120FE(-1);
}
