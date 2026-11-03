/* F_16089: read the hall of fame file (name g_4f3a, first character the drive of the game disk):
   a count word, then g_count entries. Any read failure shows the error text. A missing file
   leaves the table empty */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_1606C(void);

void F_16089(void)
{
    struct Entry *p;
    int i;
    int got;
    int fd;

    g_4f3a[0] = *g_8dd6;
    F_15FBD();
    if (_dos_open(g_4f3a, 0, &fd) || fd == 0)
        g_count = 0;
    else {
        if (_dos_read(fd, (void far *)&g_count, 2, &got) || got != 2) {
            F_1174E(g_4ee8);
            F_00FEA();
        }
        for (i = 0, p = g_table; i < g_count; p++, i++) {
            if (_dos_read(fd, (void far *)p, sizeof *p, &got) || got != sizeof *p) {
                F_1174E(g_4ee8);
                F_00FEA();
            }
        }
        _dos_close(fd);
        F_1606C();
    }
}
