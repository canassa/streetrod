/* F_00000 @ 0000:0000 (linear 0x00000, 130 bytes) */

#include <dos.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_00000(char *s)
{
    union REGS inr, outr;
    char crlf[4];
    char text[84];

    crlf[0] = '\n';
    crlf[1] = '\r';
    crlf[2] = '$';
    crlf[3] = 0;
    strcpy(text, s);
    strcat(text, crlf);
    inr.h.ah = 9;
    inr.x.dx = (int)text;
    int86(0x21, &inr, &outr);
}
