/* F_15EF9: load the game data. Builds the file name g_6c72 from the first two characters of
   g_8dd6 and the text g_4ea2 points to (g_4ea2 ends up behind its terminator), opens it and
   reads the 13 blocks of the data table (buffer g_4ea4[i], size g_4ebe[i]); any failure shows an
   error text. Ends by loading the hall of fame (F_16089) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_15EF9(void)
{
    int i;
    int got;
    int fd;
    char *drive;
    int j;

    drive = g_6c72;
    *drive++ = g_8dd6[0];
    *drive++ = g_8dd6[1];
    while (*drive++ = *g_4ea2++)
        ;
    if (_dos_open(g_6c72, 0, &fd)) {
        F_1174E("Can't open data file");
        F_00FEA();
    }
    for (i = j = 0; i < 13; i++) {
        if (!_dos_read(fd, (void far *)g_4ea4[i], g_4ebe[i], &got) && got == g_4ebe[i])
            continue;
        F_1174E("Data disc read fail");
        F_00FEA();
    }
    _dos_close(fd);
    F_16089();
}
