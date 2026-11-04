/* MEM.H - declarations of what MEM.C defines. */

#ifndef MEM_H
#define MEM_H
#include "STRUCTS.H"

extern char far *g_vramBase;                            /* DS:4C14 */
extern unsigned g_vramUsed[4];                          /* DS:4C18 */
extern int g_titlePics[];                               /* DS:4C20 */
extern int g_gamePics[];                                /* DS:4C28 */
int me_data_segment(void);                             /* 0x036A8 */
void near me_init(void);                               /* 0x036BE */
void me_free(void far *p);                             /* 0x036D6 */
void me_out_of_memory(void);                           /* 0x036E9 */
char far * me_alloc(unsigned n, int fatal);            /* 0x03709 */
struct Bitmap far *me_new_bitmap(int w, int h, int flags, int c, struct Bitmap far *mem);/* 0x0376B */
struct Bitmap far * me_wrap_bitmap(char far *bits, char far *alt, int w, int h, int flags);/* 0x03880 */
void me_set_bitmap(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags);/* 0x03904 */
char far * me_vram_alloc(unsigned size, char *pool);   /* 0x03967 */

#endif
