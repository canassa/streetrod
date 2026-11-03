/* F_120FE: mouse cursor control. mode >= 0 selects cursor shape number mode and moves it to the
   (clamped) mouse position; -1/-2 show it and -3/-4 hide it (nested by a counter). */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_120FE(int mode)
{
    int a;
    int b;

    a = g_472a;
    b = g_472c;
    a = a < 0 ? 0 : a > 319 ? 319 : a;
    b = b < 0 ? 0 : b > 199 ? 199 : b;
    if (mode >= 0) {
        g_4742 = g_06ec[g_cursor.n = mode].a;
        g_4744 = g_06ec[mode].b;
        g_cursor.off = F_11E9C(g_cursor.n);
        F_1220B(a, b);
    } else if (mode >= -2) {
        if (++g_4733 > 0) {
            g_cursor.state = (mode == -1) + 1;
            if (g_cursor.r.x2 >= 0)
                F_1220B(g_cursor.r.x1, g_cursor.r.y1);
            else
                F_1220B(a, b);
            g_cursor.state = 1;
        }
    } else {
        if (--g_4733 >= 0) {
            g_cursor.state = -((mode == -3) - 0xff);
            if (g_cursor.r.x2 >= 0)
                F_1220B(g_cursor.r.x1, g_cursor.r.y1);
            else
                F_1220B(a, b);
            g_cursor.state = 0;
        }
    }
}
