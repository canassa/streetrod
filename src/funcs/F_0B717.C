/* F_0B717 @ 0000:B717 (linear 0x0B717, 394 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0B61B(struct Dlg *, int);

static void near F_0B717(struct Dlg *d)
{
    struct Bitmap far *p;

    if (d->f0c != 1)
        F_120FE(1);
    if (d->f12) {
        F_0B61B(d, 0x472);
        if (d->f18) {
            F_11540(g_7eaa >= 10 ? -10 : -(int)g_7eaa);
            d->f18 = 0;
        }
    }
    if (d->f14 == 0) {
        F_11BD3(g_028c, 0x40, 0x64);
        if (d->f18)
            F_11540(g_7eaa >= 10 ? -10 : -(int)g_7eaa);
    }
    if (d->f10 == 0) {
        F_120FE(-3);
        p = F_15BBE(0x4d1, 2);
        F_120FE(-1);
        F_18ADB(p, 0x60, 0, 0x30, 0x40, 0xc8, 0x51, 0xa9, 2);
    }
    g_7eb0->f22 = g_7eb0->f22 > 0xaa ? 0xaa : g_7eb0->f22;
    F_181C8(2, d->f0, d->f2, (g_7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[g_4806[g_7eb0->slot12->idx].f2]);
    F_13F21(1);
    F_193D4();
    g_0284 = 1;
    F_08D26();
}
