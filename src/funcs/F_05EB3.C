/* F_05EB3 @ 0000:5EB3 (linear 0x05EB3, 55 bytes)  - true if the string is only spaces and underscores */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_05EB3(char *p)
{
    char ch;

    while ((ch = *p++) != 0)
        if (ch != ' ' && ch != '_')
            return 0;
    return 1;
}
