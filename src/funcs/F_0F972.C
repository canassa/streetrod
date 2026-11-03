/* F_0F972 @ 0F38:05F2 (linear 0x0F972, 1339 bytes)

   Draws the clock: the hour hand (width 5 or 8) and the minute hand (width 4 or 8), each a
   diamond of five lines around the face centre, from the time F_064A3 returns. */

#include <stdlib.h>
#include <math.h>
#pragma intrinsic(abs, cos, sin)
#include "GLOBALS.H"
#include "PROTOS.H"

void F_0F972(void)
{
    int a2, x2, y2, x3, x4;
    double pi3;
    unsigned soffn;
    int h4;
    double rad5;
    int y3, m, w6;
    unsigned ssegk;
    int y13, al, x14, y4, rk;

    pi3 = 3.1415926535;
    if (g_02d2 < 0)
        g_02d2 = (g_vid.mode == -2 || g_vid.mode == -6) ? 11 : 3;
    ssegk = g_787a;
    soffn = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
    F_064A3(&h4, &m);
    h4 >>= 1;
    h4 += 0x14;
    h4 = h4 % 12 + 1;
    al = 0x5a - (30 * h4 + (m >> 1));
    rk = abs(al - al / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 5;
    rad5 = al * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 13.0) + 0x91;
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 13.0) + 0x2a;
    x3 = (int)(cos((al + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((al - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_78d2)(x14, y13, x2, y2, g_02d2);
    (*g_78d2)(x14, y13, x3, y3, g_02d2);
    (*g_78d2)(x14, y13, x4, y4, g_02d2);
    (*g_78d2)(x2, y2, x3, y3, g_02d2);
    (*g_78d2)(x2, y2, x4, y4, g_02d2);
    a2 = 0x5a - 6 * m;
    rk = abs(a2 - a2 / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 4;
    rad5 = a2 * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 18.0) + 0x91;
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 18.0) + 0x2a;
    x3 = (int)(cos((a2 + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((a2 - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_78d2)(x14, y13, x2, y2, g_02d2);
    (*g_78d2)(x14, y13, x3, y3, g_02d2);
    (*g_78d2)(x14, y13, x4, y4, g_02d2);
    (*g_78d2)(x2, y2, x3, y3, g_02d2);
    (*g_78d2)(x2, y2, x4, y4, g_02d2);
    g_787a = ssegk;
    g_767c = soffn;
}
