/* F_1956E: clip the box (c, d, a, b) to 0..amax x 0..bmax, snap to the g_vid.mask pixel mask; result in out */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1956E(int a, int b, int c, int d, int amax, int bmax, struct Rect *out)
{
    if (d < 0) {
        b += d;
        d = 0;
    }
    if (d + b > bmax) {
        if ((b = bmax - d) < 0)
            b = 0;
    }
    if ((c &= g_vid.mask) < 0) {
        a += c;
        c = 0;
    }
    if (c + a > amax) {
        if ((a = amax - c) < 0)
            a = 0;
    }
    a &= g_vid.mask;
    out->x1 = c;
    out->x2 = c + a - 1;
    out->y1 = d;
    out->y2 = d + b - 1;
}
