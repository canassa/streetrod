/* F_19B6E: build the motion sequence of a sprite for a given direction */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19B6E(struct Sprite *s, int mode, int d)
{
    switch (mode) {
    case 1:
    case 4:
        if (g_0284 == 1)
            F_19A2D(s, 1, 8, 4, 2, d, 0, 0x3c);
        else
            F_19A2D(s, 1, 8, 4, 2, d, 0x19, 0x37);
        break;
    case 6:
        F_19925(s);
        F_19A2D(s, 6, -2, -4, -4, d, 0x28, 0x3c);
        break;
    case 2:
        F_19A2D(s, 2, 2, 4, 8, d, 0xa, 0x3c);
        break;
    }
}
