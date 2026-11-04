/* EGABLIT.H - declarations of what EGABLIT.C defines. */

#ifndef EGABLIT_H
#define EGABLIT_H

struct Bitmap;
struct Blt;

struct Mask {
    char far *data;
    int y;
    int h;
};

void eb_copy(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flags);
void eb_sprite(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct Mask *mask,
               struct Blt *r);
void eb_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag);
void eb_latch_pack(char far *src, int w, int h, char far *dst, int pitch);

#endif
