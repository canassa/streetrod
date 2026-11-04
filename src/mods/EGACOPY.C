#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "STRUCTS.H"
#include "EGACOPY.H"
#include "M_00D58.H"

#pragma intrinsic(outp)

int g_5c0c[5] = {0, 80, 160, 40, 120};   /* DS:5C0C */

/* F_230DC @ 21A0:16DC (linear 0x230DC, 70 bytes)

   Copy 8000 bytes between planar VGA buffers using write mode 1 (latch copy) */

void F_230DC(char far *src, char far *dst)
{
    outp(0x3ce, 5);
    outp(0x3cf, 1);
    outp(0x3c4, 2);
    outp(0x3c5, 15);
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), 8000);
    outp(0x3ce, 5);
    outp(0x3cf, 0);
}

/* F_23122 @ 21A0:1722 (linear 0x23122, 267 bytes)

   Copy 5 bands of 40 rows between planar VGA buffers, paced by the tick counter */

void F_23122(char far *dst, char far *src)
{
    int e;
    int f;
    int h;
    int g;
    long d;
    char far *a;
    char far *c;
    char far *b;

    outp(0x3c4, 2);
    outp(0x3c5, 15);
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    e = 0;
    do {
        d = g_tick + 3;
        f = 0;
        h = g_5c0c[e];
        a = dst + h;
        c = src + h;
        do {
            b = a;
            outp(0x3ce, 5);
            outp(0x3cf, 2);
            g = 40;
            do {
                *b = 0;
                b++;
                g--;
            } while (g != 0);
            outp(0x3ce, 5);
            outp(0x3cf, 1);
            movedata(FP_SEG(c), FP_OFF(c), FP_SEG(a), FP_OFF(a), 40);
            f++;
            a += 200;
            c += 200;
        } while (f < 40);
        while (d > g_tick)
            ;
        e++;
    } while (e < 5);
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    outp(0x3ce, 5);
    outp(0x3cf, 0);
}
