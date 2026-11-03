/* F_0FED1 @ 0F38:0B51 (linear 0x0FED1, 159 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_0FED1(int ticks)
{
    long end;
    char key;

    end = g_tick + ticks;
    while (end > g_tick) {
        if (g_4736) {
            g_4736 = 0;
            break;
        }
        if (g_05f2) {
            F_16D57();
            if (g_5ee)
                break;
        }
        if (g_471d) {
            F_02DF4();
            if (g_471c)
                break;
        }
        key = F_02DB6();
        if (key == 0x4d) {
            if (g_58d0) {
                F_00D99();
                g_58d0 = 0;
            } else {
                g_58d0 = 1;
                F_00DD8();
            }
        } else if (key == 0xd || key == 0x1b)
            break;
    }
}
