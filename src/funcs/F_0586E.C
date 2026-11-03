/* F_0586E @ 0000:586E (linear 0x0586E, 161 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_05795(int handle, struct Node *p);

static int near F_0586E(int handle, struct Car *c, int flag)
{
    int i;
    unsigned n;

    while (c) {
        if (_dos_write(handle, c, 0x28, &n) != 0 || n != 0x28 || F_05795(handle, c->slot6))
            return 1;
        for (i = 0; i < 5; i++)
            if (F_05795(handle, c->rack.s[i]))
                return 1;
        if (F_05795(handle, c->slot12))
            return 1;
        if (flag)
            return 0;
        c = c->f26;
    }
    return 0;
}
