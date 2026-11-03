/* F_15FBD: make sure the game disk is in the drive: tries to open the file g_6c72 (asking for
   disk 2 again and again) and closes it. From the third drive on (g_6c68 > 2, a hard disk) it
   returns the drive letter code g_6c68 + 0x40 at once. Otherwise returns the first character of
   g_6c72 */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_15409(int n);

int F_15FBD(void)
{
    int file;
    int tries;

    tries = 0;
    if (g_6c68 > 2)
        return (char)(g_6c68 + 0x40);
    for (;;) {
        if (_dos_open(g_6c72, 0, &file)) {
            if (++tries > 3)
                F_00FEA();
            g_4ee4[12] = (g_vid.mode != -2 && g_vid.mode != -6) ? '3' : '2';
            g_4ee4[25] = *g_8dd6;
            F_1174E(g_4ee4);
        } else {
            _dos_close(file);
            F_15409(1000);
            return g_6c72[0];
        }
    }
}
