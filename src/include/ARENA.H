/* ARENA.H - declarations of what ARENA.C defines. */

#ifndef ARENA_H
#define ARENA_H
#include "STRUCTS.H"

#define ARENA_LOW_BASE 0
#define ARENA_LOW_STACK 1
#define ARENA_HIGH_STACK 2
#define ARENA_HIGH_BASE 3

extern int g_arenaTopVal;                                      /* DS:78DE */
extern int g_lowArenaMarker;                                      /* DS:7B14 */
extern int g_markerIndexHigh;                                      /* DS:7D80 */
extern unsigned g_offsetArenaMarksList[];                               /* DS:8B02 */
extern int g_loHeapLimit;                                      /* DS:8F00 */
void arena_reset(void);                                 /* 0x1923C */
void arena_init(void);                                  /* 0x19269 */
void arena_shrink(int size);                            /* 0x1929A */
char far *arena_alloc(int size, int mode);              /* 0x192AC */
struct Bitmap far *arena_bm(int w, int h, int flags, int mode);/* 0x1934E */

#endif
