#include "STRUCTS.H"
#include "M_1923C.H"
#include "M_00000.H"
#include "M_036A8.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (CLAUDE.md 4f). */
static char far *g_6cd0;                   /* DS:6CD0 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (CLAUDE.md 4e). */
int bl78de;
int g7b14o;
int g7d80q;
unsigned c_8b02[61];
int o_8f00;

/* F_1923C @ 0F38:9EBC (linear 0x1923C, 45 bytes) */

void F_1923C(void)
{
    c_8b02[g7b14o = 0] = o_8f00 = 0;
    c_8b02[g7d80q = 60] = bl78de = -536;
    g_0294 = 0;
}

/* F_19269 @ 0F38:9EE9 (linear 0x19269, 49 bytes) */

void F_19269(void)
{
    if (r_vid.mode == -6)
        g_6cd0 = (char far *)0x80000000L;
    else
        g_6cd0 = F_03709(0xfde8, 1);
    F_1923C();
}

/* F_1929A @ 0F38:9F1A (linear 0x1929A, 18 bytes)

   Give back size bytes to the arena, top of the g7b14o marker stack */

void F_1929A(int size)
{
    c_8b02[g7b14o] -= size;
}

/* F_192AC @ 0F38:9F2C (linear 0x192AC, 162 bytes)

   Allocate size bytes from the arena at g_6cd0; mode picks which of four marker stacks is used */

char far *F_192AC(int size, int mode)
{
    char far *p;

    p = g_6cd0;
    switch (mode) {
    case 0:
        p += o_8f00;
        o_8f00 += size;
        c_8b02[0] = o_8f00;
        break;
    case 1:
        p += c_8b02[g7b14o];
        c_8b02[g7b14o + 1] = size + c_8b02[g7b14o];
        g7b14o++;
        break;
    case 2:
        c_8b02[g7d80q] -= size;
        p += c_8b02[g7d80q];
        c_8b02[g7d80q - 1] = c_8b02[g7d80q] - 1;
        g7d80q--;
        break;
    case 3:
        bl78de -= size;
        p += bl78de--;
        c_8b02[60] = bl78de;
        break;
    }
    return p;
}

/* F_1934E @ 0F38:9FCE (linear 0x1934E, 134 bytes)

   Allocate a bitmap w x h from the arena (header plus pixel rows) and initialise it */

struct Bitmap far *F_1934E(int w, int h, int flags, int mode)
{
    int cols, extra, total;
    unsigned size;
    struct Bitmap far *mem;
    struct Bitmap far *bm;

    cols = ((w - 1) >> r_vid.shift) + 1;
    size = cols * h;
    extra = 0;
    if (flags)
        extra = size;
    total = extra + 0x30;
    if (!(flags & 2))
        total += size * r_vid.planes;
    bm = mem = (struct Bitmap far *)F_192AC(total, mode);
    bm->bits.p = (char far *)mem + 0x30;
    F_0376B(w, h, flags, -1, bm);
    return bm;
}
