/* F_1412D: find the hot spot of the current menu (list at g_8ba6) under the point (x, y). On a hit
   the spot's box goes to g_8b90 and its id (f6) is returned; on a miss g_8b90 gets the free
   rectangle around the point and 0 is returned */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_1412D(int x, int y)
{
    struct Hot *p;
    int kind;

    p = g_8ba6;
    if (p == 0) {
        g_8b90.x1 = g_8b90.y1 = -1;
        g_8b90.x2 = g_8b90.y2 = 1000;
        g_8baa = 0;
        return 0;
    }
    g_8b90.x1 = g_8b90.y1 = 0;
    g_8b90.x2 = 319;
    g_8b90.y2 = 199;
    while ((kind = p->f0) > 9) {
        if (kind != 14) {
            if (p->x1 > x) {
                g_8b90.x2 = p->x1;
                break;
            }
            if (p->x2 <= x) {
                if (p->x2 > g_8b90.x1)
                    g_8b90.x1 = p->x2;
            } else if (p->y1 - 3 > y) {
                if (p->y1 - 3 < g_8b90.y2)
                    g_8b90.y2 = p->y1 - 3;
            } else if (p->y2 <= y) {
                if (p->y2 > g_8b90.y1)
                    g_8b90.y1 = p->y2;
            } else {
                if (kind == 12) {
                    p++;
                    continue;
                }
                g_8b90.x1 = p->x1;
                g_8b90.x2 = p->x2;
                g_8b90.y1 = p->y1;
                g_8b90.y2 = p->y2;
                g_8ba8 = p;
                return g_8baa = p->f6;
            }
            if (kind == 12)
                p = &g_09f4[p->f6];
            else
                p++;
        } else
            p++;
    }
    g_8ba8 = 0;
    return g_8baa = 0;
}
