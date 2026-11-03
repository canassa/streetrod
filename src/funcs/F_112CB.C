/* F_112CB: set EGA palette register reg to color (BIOS int 10h function 10h), waiting for vertical retrace */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp)

void F_112CB(int reg, char color)
{
    union REGS r;
    struct SREGS s;

    if (color > 7)
        color |= 0x10;
    g_palette[reg] = color;
    r.h.ah = 0x10;
    r.h.al = reg > 15;
    r.h.bl = reg;
    r.h.bh = color;
    while (!(inp(0x3da) & 8))
        ;
    int86x(0x10, &r, &r, &s);
}
