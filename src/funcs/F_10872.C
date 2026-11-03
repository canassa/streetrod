/* F_10872 @ 0F38:14F2 (linear 0x10872, 334 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_10872(struct Bitmap far *dst, struct Eng *e)
{
    struct Blt16 pkt1;
    int x, idx3;

    if (e->f24 == 0)
        idx3 = e->f18;
    else if (e->f18 == 0)
        idx3 = 0;
    else
        idx3 = e->f24 - 1;
    pkt1.h = g_6c34->h;
    if (g_6c36 >= 0) {
        pkt1.sx = g_038c[g_6c36].a;
        pkt1.sy = g_038c[g_6c36].b;
        x = g_6c34->pt[g_6c36 * 2] & 0xfff8;
        pkt1.dx = x + 0x108;
        pkt1.dy = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[g_6c36 * 2 + 1] + 0x17;
        pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
        (*g_78a2)(g_82b4, dst, (struct Blt *)&pkt1, 1);
    }
    pkt1.sx = g_038c[idx3].a;
    pkt1.sy = g_038c[idx3].b + 0x13;
    x = g_6c34->pt[idx3 * 2] & 0xfff8;
    pkt1.dx = x + 0x108;
    pkt1.dy = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[idx3 * 2 + 1] + 0x17;
    pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
    (*g_78a2)(g_82b4, dst, (struct Blt *)&pkt1, 1);
    g_6c36 = idx3;
    e->f1a = 0;
}
