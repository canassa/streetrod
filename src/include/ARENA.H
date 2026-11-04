/* ARENA.H - declarations of what ARENA.C defines. */

#ifndef ARENA_H
#define ARENA_H
#include "STRUCTS.H"

extern int bl78de;                                      /* DS:78DE */
extern int g7b14o;                                      /* DS:7B14 */
extern int g7d80q;                                      /* DS:7D80 */
extern unsigned c_8b02[];                               /* DS:8B02 */
extern int o_8f00;                                      /* DS:8F00 */
void arena_reset(void);                                 /* 0x1923C */
void arena_init(void);                                  /* 0x19269 */
void arena_shrink(int size);                            /* 0x1929A */
char far *arena_alloc(int size, int mode);              /* 0x192AC */
struct Bitmap far *F_1934E(int w, int h, int flags, int mode);/* 0x1934E */

#endif
