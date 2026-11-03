/* F_01111 @ 0000:1111 (linear 0x01111, 461 bytes) */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(inp, outp)

void F_01111(void)
{
    int sumx;
    int sumy;
    int first;
    int v;
    int j;

    sumx = 0;
    sumy = 0;
    g_8ef6 = 10;
    g_8ef8 = 2;
    g_8efa = 5;
    g_8efc = 5;
    outp(0x201, 0);
    outp(0x201, 0);
    first = inp(0x201);
    for (j = 1; j; j++)
        if (((v = inp(0x201)) & 0xf3) == 0xf0)
            break;
    if (g_0610 == 0 && j != 0 && v != first) {
        g_471d = 1;
        F_02DF4();
        sumx = g_4718;
        sumy = g_471a;
        F_02DF4();
        sumx += g_4718;
        sumy += g_471a;
        F_02DF4();
        sumx += g_4718;
        sumy += g_471a;
        g_471e = sumx / 3;
        g_4720 = sumy / 3;
        g_4722 = -(g_471e >> 1);
        g_4724 = g_471e >> 1;
        g_4726 = -(g_4720 >> 1);
        g_4728 = g_4720 >> 1;
    }
    if (g_0610 == 0 && g_0262 && F_16D1A() == -1) {
        g_05f2 = 1;
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 1;
    } else
        g_05f2 = 0;
    g_6c46 = -1;
    g_6c48 = -1;
    g_79aa.off = 0;
    g_79aa.l8 = g_2e6b_0000;
    g_79aa.cur = g_79aa.lc = g_79aa.l8;
    g_79aa.f16 = g_79aa.f14 = g_5858;
    g_79aa.f1a = g_79aa.f18 = g_5882;
    g_79c6.off = 0;
    g_79c6.f1a = g_79c6.f18 = g_58cc;
    _harderr(F_0105B);
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0x40);
    g_7aaa = _dos_getvect(8);
    _dos_setvect(8, F_01FE2);
    _enable();
    F_02DC3();
    g_05e0 = 1;
    outp(0x43, 0xb6);
    F_00E24();
    g_0044 = 1;
}
