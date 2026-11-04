#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "STRUCTS.H"
#include "EGACOPY.H"
#include "PC.H"
#include "SYS.H"

int g_revealOffsets[5] = {0, 80, 160, 40, 120};   /* DS:5C0C */

/* egacopy_page @ 21A0:16DC (linear 0x230DC, 70 bytes)

   Copy 8000 bytes between planar VGA buffers using write mode 1 (latch copy) */

void egacopy_page(char far *src, char far *dst)
{
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE1);
    outp(SEQ_INDEX, SEQ_MAP_MASK);
    outp(SEQ_DATA, SEQ_ALL_PLANES);
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), 8000);
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE0);
}

/* egacopy_reveal @ 21A0:1722 (linear 0x23122, 267 bytes)

   Copy 5 bands of 40 rows between planar VGA buffers, paced by the tick counter */

void egacopy_reveal(char far *dst, char far *src)
{
    int e;
    int f;
    int h;
    int g;
    long d;
    char far *a;
    char far *c;
    char far *b;

    outp(SEQ_INDEX, SEQ_MAP_MASK);
    outp(SEQ_DATA, SEQ_ALL_PLANES);
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    e = 0;
    do {
        d = g_tick + 3;
        f = 0;
        h = g_revealOffsets[e];
        a = dst + h;
        c = src + h;
        do {
            b = a;
            outp(GC_INDEX, GC_MODE);
            outp(GC_DATA, GC_WRITE_MODE2);
            g = EGA_PITCH;
            do {
                *b = 0;
                b++;
                g--;
            } while (g != 0);
            outp(GC_INDEX, GC_MODE);
            outp(GC_DATA, GC_WRITE_MODE1);
            movedata(FP_SEG(c), FP_OFF(c), FP_SEG(a), FP_OFF(a), EGA_PITCH);
            f++;
            a += 200;
            c += 200;
        } while (f < 40);
        while (d > g_tick)
            ;
        e++;
    } while (e < 5);
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE0);
}
