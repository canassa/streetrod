/* F_19A2D: split a move of size d into the three phases of a sprite's motion sequence */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(abs)

void F_19A2D(struct Sprite *s, int mode, int a, int b, int c, int d, int e, int f)
{
    int n1, n2, n3;

    switch (mode) {
    case 1:
    case 4:
        n3 = -(e + f - 100) * d / 100 / c & 0xfffc;
        n2 = f * d / 100 / b & 0xfffe;
        n1 = (d - n2 * b - n3 * c) / a;
        break;
    case 2:
    case 6:
        n1 = abs(e * d / 100 / a) & 0xfffc;
        n2 = abs(f * d / 100 / b) & 0xfffe;
        n3 = abs((d - abs(a) * n1 - abs(b) * n2) / c);
        break;
    }
    F_199DD(s, 0, n1 + 1, (char)a, 0);
    F_199DD(s, 1, n2, (char)b, 0);
    F_199DD(s, 2, n3, (char)c, 0);
    F_199DD(s, 3, -1, 0, 0);
}
