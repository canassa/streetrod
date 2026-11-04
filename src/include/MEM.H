/* MEM.H - declarations of what MEM.C defines. */

#ifndef MEM_H
#define MEM_H
#include "STRUCTS.H"

extern char far *g_4c14;                                /* DS:4C14 */
extern unsigned g_4c18[4];                              /* DS:4C18 */
extern int g_4c20[];                                    /* DS:4C20 */
extern int g_4c28[];                                    /* DS:4C28 */
int data_segment(void);                                 /* 0x036A8 */
void near F_036BE(void);                                /* 0x036BE */
void F_036D6(void far *p);                              /* 0x036D6 */
void out_of_memory(void);                               /* 0x036E9 */
char far * F_03709(unsigned n, int fatal);              /* 0x03709 */
struct Bitmap far *F_0376B(int w, int h, int flags, int c, struct Bitmap far *mem);/* 0x0376B */
struct Bitmap far * F_03880(char far *bits, char far *alt, int w, int h, int flags);/* 0x03880 */
void F_03904(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags);/* 0x03904 */
char far * F_03967(unsigned size, char *pool);          /* 0x03967 */

#endif
