#include <malloc.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"


char far *F_03709(unsigned n, int fatal)
{
    char far *p;

    if (p = _fmalloc(n)) {
        if (FP_SEG(p) == g_6c60 ||
            (g_vid.mode == -6 && (unsigned long)(p + n) >= 0x80000000UL))
            _ffree(p);
        else
            return p;
    }
    if (fatal)
        out_of_memory();
    else
        return 0;
}
