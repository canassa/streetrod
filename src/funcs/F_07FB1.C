/* F_07FB1 @ 0000:7FB1 (linear 0x07FB1, 506 bytes) */

#include <conio.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp, abs)

static void near F_07FB1(struct Car *car)
{
    struct Obj *p;
    long tmp;
    struct Blt blt;
    union Word w;
    int key;
    int y;
    unsigned n;
    int gear;

    F_13DE6(0x1c);
    blt.w = 8;
    blt.h = 11;
    blt.sx = 0x30;
    blt.sy = 0;
    blt.dx = 0xb0;
    F_119B8(0x42f, 0x60, 9, -1);
    p = g_7568 + F_11149(8);
    tmp = g_0600;
    F_00D99();
    g_58d4 = 2;
    if (g_47ce) {
        outp(0x61, inp(0x61) | 3);
        g_47cf = 1;
    }
    do {
        if (g_47ce && g_47ce != g_47cf) {
            outp(0x61, inp(0x61) | 3);
            g_47cf = 1;
        }
        gear = car->f20;
        w.w = (abs(gear) << 10) + 30000;
        outp(0x42, w.b[0]);
        outp(0x42, w.b[1]);
        y = blt.dy = gear * 2 + 0x4b;
        F_120FE(-4);
        (*g_78a2)(p->img, g_vid.back, &blt, 1);
        F_1A92A(g_vid.back, g_vid.front, 0xb0, y, 0xb7, y + 10);
        F_120FE(-2);
        while ((key = F_01417(0)) == 0) {
            if (g_47ce && g_47ce != g_47cf) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
            }
        }
        switch (key) {
        case -10:
            car->f20++;
            if (car->f20 > 4)
                car->f20 = 4;
            break;
        case -7:
            car->f20--;
            if (car->f20 < -8)
                car->f20 = -8;
            break;
        }
    } while (key != -1);
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    g_58d4 = 1;
    F_00DD8();
    F_10D6C(8);
    n = ((unsigned)g_0600 - (unsigned)tmp) * 3;
    n = n > 0x444 ? 0x444 : n;
    g_05fc += n;
    F_13F21(1);
}
