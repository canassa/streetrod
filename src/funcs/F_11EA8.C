/* F_11EA8: set up the off-screen work bitmap and the scratch strip used by the pop-up code */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_11EA8(void)
{
    unsigned bytes;
    int size;
    struct Bitmap far *mem;
    struct Bitmap far *img;
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    unsigned far *fp;
    int step;

    bytes = 0x600;
    size = (g_vid.planes + 1) * bytes + 0x60;
    if (g_vid.mode != -2)
        size += (0x18 >> g_vid.shift) << 4;
    mem = (struct Bitmap far *)F_03709(size, 1);
    mem->bits.p = (char far *)mem + 0x30;
    img = F_0376B(0x18 << g_vid.shift, 0x40, 1, -1, mem);
    g_cursor.save = (struct Bitmap far *)(img->alt.p + bytes);
    F_03904(g_cursor.save, g_vid.mode == -2 ? (char far *)0xa0001f40L : (char far *)(g_cursor.save + 1), 0L,
            (0x18 >> g_vid.shift) << g_vid.shift, 0x10, g_vid.mode == -2 ? -2 : -1);
    memset(&g_cursor.clip, 0, 12);
    g_cursor.state = 0;
    pict = F_15BBE(2, 2);
    if (pict == 0)
        out_of_memory();
    blt.w = 0x18;
    blt.h = 0x40;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_78a2)(pict, img, &blt, 3);
    for (step = 1; step <= ~g_vid.mask; step++) {
        tmp = F_1934E(0x18, 0x40, 1, 1);
        F_1A03F(pict, step, tmp);
        blt.dx += 0x18;
        (*g_78a2)(tmp, img, &blt, 3);
        F_1941E(1);
    }
    F_193D4();
    g_cursor.shape = img->bits.p;
    g_cursor.mask = img->alt.p;
    if (g_vid.mode == -2) {
        fp = (unsigned far *)g_cursor.mask;
        step = img->planesize >> 3;
        while (step--) {
            *fp = ~*fp;
            fp++;
        }
    }
    g_cursor.size = img->pitch << 4;
    g_cursor.n = 1;
    g_cursor.off = F_11E9C(1);
    g_cursor.r.x2 = -1;
    g_472a = 0x9e;
    g_472c = 0x80;
    g_4746 = 0;
    g_4748 = 0;
    g_8247 = 2;
}
