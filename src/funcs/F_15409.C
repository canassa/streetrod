/* F_15409: make sure the picture file holding picture number n is open (the pictures come in two
   files, split at g_8bc8). Closes the other one, asks for the right disk until the open works
   and checks the file header word */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_153D7(int n);

static void near F_15409(int n)
{
    int kind;
    int tries;
    unsigned got;

    if (F_153D7(n) >= g_8bc8) {
        kind = 2;
        g_6c6e = g_8dd6;
        g_6c6c = &g_4eda;
        g_6c70 = &g_4ed8;
        g_6c6a = &g_8dd2;
    } else {
        kind = 1;
        g_6c6e = g_8dd4;
        g_6c6c = &g_4ed8;
        g_6c70 = &g_4eda;
        g_6c6a = &g_8bc8;
    }
    if (*g_6c6c == 0) {
        _dos_close(*g_6c70);
        *g_6c70 = 0;
        g_4ee4[12] = kind == 1 ? '1' : (g_vid.mode != -2 && g_vid.mode != -6) ? '3' : '2';
        g_4ee4[25] = *g_6c6e;
        tries = 0;
        while (_dos_open(g_6c6e, 0, g_6c6c)) {
            if (tries++ == 3)
                F_00FEA();
            F_1174E(g_4ee4);
        }
        if (_dos_read(*g_6c6c, (void far *)g_6c6a, 2, &got) || got != 2 || g_8bc8 + g_8dd2 != 0x115) {
            F_1174E(g_4ee8);
            F_00FEA();
        }
    }
}
