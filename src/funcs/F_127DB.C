/* F_127DB: redraw the dialog picture n and its two bars; s gives the bar states.
   The screen shadow flag is switched off while drawing and the area is copied to the front afterwards. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_127DB(int n, char far *s)
{
    int saved;
    int i;

    saved = g_vid.f16;
    if (n) {
        g_06fc = 0;
        g_vid.f16 = 0;
        F_120FE(-3);
        F_1264E(n, 0);
        for (i = 0; i < 2; i++)
            F_123F4(*s++, i);
        F_120FE(-1);
        if (g_vid.f16 = saved)
            F_1A92A(g_vid.back, g_vid.front, 0x20, 10, 0xe7, 0xb8);
        g_06fc = 1;
    }
}
