/* F_13F9A: bounding box of hot spot id and the spots linked after it (through f10), widened by 3
   and clipped to the 320 x 200 screen. *x and *y receive the top left corner, *w and *h the size */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_13F9A(int id, int *x, int *y, int *w, int *h)
{
    struct Hot *p;

    p = &g_09f4[g_2bad[id]];
    g_6c56 = g_6c58 = 1000;
    g_6c5a = g_6c5c = 1;
    while (p->f10) {
        p = &g_09f4[p->f10];
        g_6c56 = p->x1 > g_6c56 ? g_6c56 : p->x1;
        g_6c5a = p->x2 < g_6c5a ? g_6c5a : p->x2;
        g_6c58 = p->y1 > g_6c58 ? g_6c58 : p->y1;
        g_6c5c = p->y2 < g_6c5c ? g_6c5c : p->y2;
    }
    if ((*x = g_6c56 - 3) < 0)
        *x = 0;
    if ((*y = g_6c58 - 3) < 0)
        *y = 0;
    if ((*w = g_6c5a + 3) > 320)
        *w = 320;
    if ((*h = g_6c5c + 3) > 200)
        *h = 200;
    *w -= *x;
    *h -= *y;
}
