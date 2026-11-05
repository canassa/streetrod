/* ARENA.H - declarations of what ARENA.C defines. */

#ifndef ARENA_H
#define ARENA_H
#define ARENA_INITIAL_TOP (-536)
#define ARENA_LAST_MARK 60
#define ARENA_BYTES 0xfde8

struct Bitmap;

enum ArenaMode {
    ARENA_LOW_BASE = 0,
    ARENA_LOW_STACK = 1,
    ARENA_HIGH_STACK = 2,
    ARENA_HIGH_BASE = 3
};

extern int g_arenaTopVal;
extern int g_lowArenaMarker;
extern int g_markerIndexHigh;
extern unsigned g_offsetArenaMarksList[];
extern int g_loHeapLimit;
void ar_reset(void);
void ar_init(void);
void ar_shrink(int size);
char far *ar_alloc(int size, enum ArenaMode mode);
struct Bitmap far *ar_bm(int w, int h, int flags, enum ArenaMode mode);

#endif
