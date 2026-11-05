#include "ARENA.H"
#include "BITMAP.H"
#include "MAIN.H"
#include "MEM.H"

#define ARENA_FIXED_BASE ((char far *)0x80000000L)

static char far *g_arenaBase;

int g_arenaTopVal;
int g_lowArenaMarker;
int g_markerIndexHigh;
unsigned g_offsetArenaMarksList[ARENA_LAST_MARK + 1];
int g_loHeapLimit;


void ar_reset(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit = 0;
    g_offsetArenaMarksList[g_markerIndexHigh = ARENA_LAST_MARK] = g_arenaTopVal = ARENA_INITIAL_TOP;
    g_stickerBg = 0;
}


void ar_init(void)
{
    if (r_vid.mode == VID_TANDY)
        g_arenaBase = ARENA_FIXED_BASE;
    else
        g_arenaBase = me_alloc(ARENA_BYTES, 1);
    ar_reset();
}

/* Give back size bytes to the arena, top of the g_lowArenaMarker marker stack */

void ar_shrink(int size)
{
    g_offsetArenaMarksList[g_lowArenaMarker] -= size;
}

/* Allocate size bytes from the arena at g_arenaBase; mode picks which of four marker stacks is used */

char far *ar_alloc(int size, enum ArenaMode mode)
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
        g_offsetArenaMarksList[ARENA_LAST_MARK] = g_arenaTopVal;
        break;
    }
    return p;
}

/* Allocate a bitmap w x h from the arena (header plus pixel rows) and initialise it */

struct Bitmap far *ar_bm(int w, int h, int flags, enum ArenaMode mode)
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
    total = extra + BITMAP_HEADER_BYTES;
    if (!(flags & 2))
        total += size * r_vid.planes;
    bm = mem = (struct Bitmap far *)ar_alloc(total, mode);
    bm->bits.p = (char far *)mem + BITMAP_HEADER_BYTES;
    me_new_bitmap(w, h, flags, BITMAP_WORK, bm);
    return bm;
}
