/* F_1287B: slot number for the item in g_8290: 0x434..0x436 map to 0..2 (anything else is returned as it is) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1287B(void)
{
    switch (g_8290[0]) {
    case 0x434:
        return 0;
    case 0x435:
        return 1;
    case 0x436:
        return 2;
    }
}
