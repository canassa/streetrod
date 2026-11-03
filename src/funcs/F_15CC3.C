/* F_15CC3: open the two picture files (names g_8dd4 and g_8dd6, drive letter patched into their
   first character) and read the entry counts g_8bc8 and g_8dd2 from their headers. disks is the
   number of disk drives wanted: 2 only when the equipment word says there are two floppies. mode 2
   and 5 pick the other file names. Any failure shows an error text */

#include <bios.h>
#include <dos.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_15CC3(int disks, int mode)
{
    int equip;
    int tries;
    int err;

    _dos_getdrive((unsigned *)&g_6c68);
    if (g_6c68 < 3 && disks > 1 && (equip = _bios_equiplist(), equip & 1) && (equip & 0xc0))
        disks = 2;
    else
        disks = 1;
    g_8dd4 = (mode == 2 || mode == 5) ? g_4edc : g_4ede;
    *g_8dd4 = g_6c68 + 0x40;
    err = _dos_open(g_8dd4, 0, &g_4ed8);
    if (err || g_4ed8 == 0) {
        if (g_6c68 > 2) {
            F_1174E(g_4ee6);
        } else {
            strcpy(g_4eec + 0x10, g_8dd4 + 2);
            F_1174E(g_4eec);
        }
        F_00FEA();
    }
    if (_dos_read(g_4ed8, (void far *)&g_8bc8, 2, &err) || err != 2) {
        F_1174E(g_4ee8);
        F_00FEA();
    }
    g_8dd6 = (mode == 2 || mode == 5) ? g_4ee0 : g_4ee2;
    *g_8dd6 = *g_8dd4;
    if (_dos_open(g_8dd6, 0, &g_4eda) || g_4eda == 0) {
        if (g_6c68 > 2) {
            F_1174E(g_4ee6);
            F_00FEA();
        } else if (disks == 1) {
            g_4eda = 0;
        } else {
            *g_8dd6 = g_6c68 == 1 ? 'B' : 'A';
            err = _dos_open(g_8dd6, 0, &g_4eda);
            for (tries = 0; err || g_4eda == 0; tries++) {
                if (tries == 3)
                    F_00FEA();
                g_4ee4[12] = (mode != 2 && mode != 5) ? '3' : '2';
                g_4ee4[0x19] = *g_8dd6;
                F_1174E(g_4ee4);
                err = _dos_open(g_8dd6, 0, &g_4eda);
            }
        }
    }
    if (g_4eda) {
        if (_dos_read(g_4eda, (void far *)&g_8dd2, 2, &err) || err != 2 || g_8bc8 + g_8dd2 != 0x115) {
            F_1174E(g_4ee8);
            F_00FEA();
        }
    }
}
