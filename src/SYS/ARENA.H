/* ARENA.H - declarations of what ARENA.C defines. */

#ifndef ARENA_H
#define ARENA_H
#include "STRUCTS.H"

#define ARENA_LOW_BASE 0
#define ARENA_LOW_STACK 1
#define ARENA_HIGH_STACK 2
#define ARENA_HIGH_BASE 3

extern int g_arenaTopVal;
extern int g_lowArenaMarker;
extern int g_markerIndexHigh;
extern unsigned g_offsetArenaMarksList[];
extern int g_loHeapLimit;
void ar_reset(void);
void ar_init(void);
void ar_shrink(int size);
char far *ar_alloc(int size, int mode);
struct Bitmap far *ar_bm(int w, int h, int flags, int mode);

#endif
