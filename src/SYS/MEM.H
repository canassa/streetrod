/**
 * @file MEM.H
 * @brief Bitmap allocation, video storage and memory management.
 */

#ifndef MEM_H
#define MEM_H

#include "BITMAP.H"

struct Bitmap;

extern int g_titlePics[];
extern int g_gamePics[];
int me_data_segment(void);
void near me_init(void);
void me_free(void far *p);
void me_out_of_memory(void);
char far *me_alloc(unsigned n, int fatal);
struct Bitmap far *me_new_bitmap(int w, int h, int flags, enum BitmapKind c, struct Bitmap far *mem);
struct Bitmap far *me_wrap_bitmap(char far *bits, char far *alt, int w, int h, enum BitmapKind flags);
void me_set_bitmap(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, enum BitmapKind flags);
char far *me_vram_alloc(unsigned size, char *pool);

#endif
