#include "STRUCTS.H"
#include "ARENA.H"
#include "MAIN.H"
#include "MEM.H"

#define ARENA_FIXED_BASE ((char far *)0x80000000L)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static char far *g_arenaBase;                   /* DS:6CD0 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int g_arenaTopVal;
int g_lowArenaMarker;
int g_markerIndexHigh;
unsigned g_offsetArenaMarksList[61];
int g_loHeapLimit;

/* arena_reset @ 0F38:9EBC (linear 0x1923C, 45 bytes) */

void arena_reset(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit = 0;
    g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal = -536;
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

   Give back size bytes to the arena, top of the g_lowArenaMarker marker stack */

void arena_shrink(int size)
{
    g_offsetArenaMarksList[g_lowArenaMarker] -= size;
}

/* arena_alloc @ 0F38:9F2C (linear 0x192AC, 162 bytes)

   Allocate size bytes from the arena at g_arenaBase; mode picks which of four marker stacks is used */

char far *arena_alloc(int size, int mode)
{
    char far *p;

    p = g_arenaBase;
    switch (mode) {
    case ARENA_LOW_BASE:
        p += g_loHeapLimit;
        g_loHeapLimit += size;
        g_offsetArenaMarksList[0] = g_loHeapLimit;
        break;
    case ARENA_LOW_STACK:
        p += g_offsetArenaMarksList[g_lowArenaMarker];
        g_offsetArenaMarksList[g_lowArenaMarker + 1] = size + g_offsetArenaMarksList[g_lowArenaMarker];
        g_lowArenaMarker++;
        break;
    case ARENA_HIGH_STACK:
        g_offsetArenaMarksList[g_markerIndexHigh] -= size;
        p += g_offsetArenaMarksList[g_markerIndexHigh];
        g_offsetArenaMarksList[g_markerIndexHigh - 1] = g_offsetArenaMarksList[g_markerIndexHigh] - 1;
        g_markerIndexHigh--;
        break;
    case ARENA_HIGH_BASE:
        g_arenaTopVal -= size;
        p += g_arenaTopVal--;
        g_offsetArenaMarksList[60] = g_arenaTopVal;
        break;
    }
    return p;
}

/* arena_bm @ 0F38:9FCE (linear 0x1934E, 134 bytes)

   Allocate a bitmap w x h from the arena (header plus pixel rows) and initialise it */

struct Bitmap far *arena_bm(int w, int h, int flags, int mode)
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
