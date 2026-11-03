/* F_1633D @ 0F38:6FBD: if on, fill the first entry of the sound table; g_72ec = on either way */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1633D(int on, int a, int b, int c, int d)
{
    if (on) {
        g_tones[0].f0 = 0;
        g_tones[0].f1 = a;
        g_tones[0].f2 = c;
        g_tones[0].f3 = b;
        g_tones[0].f4 = d;
    }
    g_72ec = on;
}
