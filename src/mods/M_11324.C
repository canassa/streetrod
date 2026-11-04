#include <dos.h>
#include "STRUCTS.H"
#include "M_10B7C.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
char iz8248;

/* F_11324: load all 16 EGA palette registers plus the overscan color (BIOS int 10h function 10h, al = 2) */


void F_11324(char *table)
{
    char pal[17];
    char far *dest;
    union REGS r;
    struct SREGS s;
    int n;

    dest = (char far *)pal;
    for (n = 0; n < 16; n++) {
        if (*table > 7)
            *dest++ = *table | 0x10;
        else
            *dest++ = *table;
        table++;
    }
    pal[16] = iz8248;
    dest = (char far *)pal;
    s.es = FP_SEG(dest);
    r.x.dx = FP_OFF(dest);
    r.h.ah = 0x10;
    r.h.al = 2;
    int86x(0x10, &r, &r, &s);
}
