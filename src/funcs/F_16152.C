/* F_16152: write the hall of fame file (g_4f3a): the table is scrambled (F_1604F), the file made
   (asking for the disk until it works on a floppy), the count word and the g_count entries
   written. Any failure shows the error text and deletes the file. The table is unscrambled at
   the end (F_1606C) */

#include <dos.h>
#include <io.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_1604F(void);
static void near F_1606C(void);

void F_16152(void)
{
    struct Entry *p;
    int tries;
    int i;
    unsigned got;
    int fd;

    F_1604F();
    F_15FBD();
    got = _dos_creat(g_4f3a, 0, &fd);
    if (g_6c68 > 2) {
        if (fd == 0 || got) {
            F_1174E(g_4eea);
            F_00FEA();
        }
    } else {
        g_4ee4[12] = '1';
        g_4ee4[25] = g_4f3a[0];
        for (tries = 0; fd == 0 || got; tries++) {
            if (tries == 3)
                F_00FEA();
            F_1174E(g_4ee8);
            got = _dos_creat(g_4f3a, 0, &fd);
        }
    }
    if (_dos_write(fd, (void far *)&g_count, 2, &got) || got != 2) {
bad:
        F_1174E(g_4eea);
        unlink(g_4f3a);
        F_1174E(g_4eea);
    } else {
        for (i = 0, p = g_table; i < g_count; p++, i++)
            if (_dos_write(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p)
                goto bad;
        if (_dos_close(fd))
            goto bad;
    }
    F_1606C();
}
