#include "STRUCTS.H"
#include "ARENA.H"
#include "MAIN.H"
#include "MEM.H"

#define ARENA_FIXED_BASE ((char far *)0x80000000L)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static char far *g_arenaBase;                   /* DS:6CD0 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int bl78de;
int g7b14o;
int g7d80q;
unsigned c_8b02[61];
int o_8f00;

/* arena_reset @ 0F38:9EBC (linear 0x1923C, 45 bytes) */

void arena_reset(void)
{
    c_8b02[g7b14o = 0] = o_8f00 = 0;
    c_8b02[g7d80q = 60] = bl78de = -536;
    g_0294 = 0;
}

/* arena_init @ 0F38:9EE9 (linear 0x19269, 49 bytes) */

void arena_init(void)
{
    if (r_vid.mode == VID_TANDY)
        g_arenaBase = ARENA_FIXED_BASE;
    else
        g_arenaBase = mem_alloc(0xfde8, 1);
    arena_reset();
}

/* arena_shrink @ 0F38:9F1A (linear 0x1929A, 18 bytes)

   Give back size bytes to the arena, top of the g7b14o marker stack */

void arena_shrink(int size)
{
    c_8b02[g7b14o] -= size;
}

/* arena_alloc @ 0F38:9F2C (linear 0x192AC, 162 bytes)

   Allocate size bytes from the arena at g_arenaBase; mode picks which of four marker stacks is used */

char far *arena_alloc(int size, int mode)
{
    char far *p;

    p = g_arenaBase;
    switch (mode) {
    case ARENA_LOW_BASE:
        p += o_8f00;
        o_8f00 += size;
        c_8b02[0] = o_8f00;
        break;
    case ARENA_LOW_STACK:
        p += c_8b02[g7b14o];
        c_8b02[g7b14o + 1] = size + c_8b02[g7b14o];
        g7b14o++;
        break;
    case ARENA_HIGH_STACK:
        c_8b02[g7d80q] -= size;
        p += c_8b02[g7d80q];
        c_8b02[g7d80q - 1] = c_8b02[g7d80q] - 1;
        g7d80q--;
        break;
    case ARENA_HIGH_BASE:
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
    bm = mem = (struct Bitmap far *)arena_alloc(total, mode);
    bm->bits.p = (char far *)mem + 0x30;
    mem_new_bitmap(w, h, flags, -1, bm);
    return bm;
}
