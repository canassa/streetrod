/* F_0D544 @ 0000:D544 (linear 0x0D544, 224 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_0D544(int rpm)
{
    if (rpm <= 0)
        return 0;
    if (g_8acc < 4) {
        if (rpm <= 0x47)
            return (rpm >> 3) + 1;
        return (rpm >> 2) - 8;
    }
    if (rpm <= 0x61) {
        if (rpm <= 0x37) {
            if (rpm <= 0x1b) {
                if (rpm <= 0xd)
                    return 1;
                return 2;
            }
            if (rpm <= 0x29)
                return 3;
            return 4;
        }
        if (rpm <= 0x53) {
            if (rpm <= 0x45)
                return 5;
            return 6;
        }
        return 7;
    }
    if (rpm <= 0x71) {
        if (rpm <= 0x69) {
            if (rpm <= 0x65)
                return 8;
            return 9;
        }
        if (rpm <= 0x6d)
            return 10;
        return 11;
    }
    if (rpm <= 0x79) {
        if (rpm <= 0x75)
            return 12;
        return 13;
    }
    if (rpm <= 0x81) {
        if (rpm <= 0x7d)
            return 14;
        return 15;
    }
    return 16;
}
