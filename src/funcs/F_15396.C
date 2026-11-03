/* F_15396: seek the DOS file handle to the absolute position pos (INT 21h function 42h, method 0).
   Returns 0 on success, else the error in g_6318, also copied to g_630d */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

int F_15396(int handle, long pos)
{
    union REGS in, out;

    in.x.ax = 0x4200;
    in.x.bx = handle;
    in.x.cx = (int)(pos >> 16);
    in.x.dx = (int)pos;
    intdos(&in, &out);
    if (out.x.cflag == 0)
        return 0;
    return g_630d = g_6318;
}
