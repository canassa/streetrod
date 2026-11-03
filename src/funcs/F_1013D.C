/* F_1013D @ 0F38:0DBD (linear 0x1013D, 406 bytes) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

void F_1013D(int mode)
{
    switch (mode) {
    case 0:
        if (!g_02ea) {
            g_02ea = g_vid.front;
            g_02e6 = g_vid.back;
        }
        g_004c = 1;
        break;
    case 1:
        g_vid.front = g_02ea;
        g_vid.back = g_02e6;
        if (g_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            F_21A14(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        F_113A2(0xbe);
        break;
    case 2:
        if (g_vid.mode == -2) {
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        break;
    case 3:
        g_vid.back = g_vid.front = g_02ea;
        g_vid.f16 = 0;
        F_113A2(g_vid.mode == -2 ? 0x5a : 0xbe);
        if (g_8aca)
            F_11593(g_49de);
        break;
    case 4:
        g_vid.front = g_02ea;
        g_vid.back = g_02e6;
        if (g_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            F_21A14(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        (*g_present)();
        break;
    }
}
