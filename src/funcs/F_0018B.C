/* F_0018B @ 0000:018B (linear 0x0018B, 70 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0018B(void)
{
    union REGS r;
    int s;
    int c;

    r.h.ah = 0x1a;
    r.h.al = 0;
    int86(0x10, &r, &r);
    s = r.h.al;
    c = r.h.bl;
    return s && c == 8;
}
