/* F_05795 @ 0000:5795 (linear 0x05795, 71 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_05795(int handle, struct Node *p)
{
    unsigned written;

    for (; p; p = p->next) {
        if (_dos_write(handle, p, 8, &written) != 0 || written != 8)
            return 1;
    }
    return 0;
}
