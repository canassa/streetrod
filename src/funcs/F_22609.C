/* F_22609: copy a 32K block (a full EGA/VGA-sized buffer) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_22609(char far *src, char far *dst)
{
    F_1A882(src, dst, 0x8000);
}
