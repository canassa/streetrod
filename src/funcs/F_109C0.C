/* F_109C0 @ 0F38:1640 (linear 0x109C0, 64 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_109C0(char far *p, int c, unsigned n)
{
    char far *q;

    for (q = p; n; n--)
        *q++ = c;
}
