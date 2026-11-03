/* F_1552B: read the picture headers of one picture file into g_slots: which 1 is the first file,
   anything else the second (its entries follow the first file's, g_8bc8 of them). Clears the
   run time fields of each entry. Returns the largest data size found */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_15409(int n);

int F_1552B(int which)
{
    int num;
    unsigned height;
    int i;
    int count;
    int got;
    int base;
    struct Slot far *sl;

    base = which == 1 ? 0 : g_8bc8;
    num = which == 1 ? 1 : 1000;
    height = 0;
    F_15409(num);
    count = which == 1 ? g_8bc8 : g_8dd2;
    for (i = 0; i < count; i++) {
        sl = &g_slots[base + i];
        if (F_15396(*g_6c6c, (long)(i * 0x1e + 2)) ||
            _dos_read(*g_6c6c, (void far *)sl, 0x1e, &got) || got != 0x1e) {
            F_1174E(g_4ee8);
            F_00FEA();
        }
        sl->f1e = 0xff;
        sl->ptr.p = 0;
        sl->f24 = 0;
        sl->f28 = 0;
        sl->f2c = -1;
        sl->f2e = 1;
        if (sl->f1c > height)
            height = sl->f1c;
    }
    return height;
}
