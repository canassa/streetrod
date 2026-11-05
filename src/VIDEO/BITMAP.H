/**
 * @file BITMAP.H
 * @brief Bitmap storage and clipped image composition.
 */

#ifndef BITMAP_H
#define BITMAP_H
/** Bytes preceding pixel storage in an allocated bitmap. */
#define BITMAP_HEADER_BYTES 0x30


/* blit request handed to the g_videoOps hooks (20 bytes). The first six ints are the same as
   struct Clip; da_clip_blt reads only those. mx, my: origin in the mask bitmap (tb_compose). */
struct Blt {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int unused1;
    int unused2;
    int mx;
    int my;
};

union FarPtr {
    char far *p;
    struct {
        unsigned off;
        unsigned seg;
    } w;
};

struct Bitmap {
    int width;
    int height;
    unsigned planesize;
    char unused[0x1e];
    union FarPtr bits;
    union FarPtr alt;
    int pitch;
    signed char flags;
};

struct Rect {
    int x1;
    int y1;
    int x2;
    int y2;
};

/* clipped rectangle (12 bytes): size, source origin, destination origin. da_clip_blt takes struct Blt *
   but reads and writes only these first 12 bytes, so callers with a Clip local pass it cast. */
struct Clip {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
};

/* 16-byte blit packet of da_draw_shifter (struct Blt without the last two words) */
struct Blt16 {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int unused1;
    int unused2;
};

void bm_copy(struct Bitmap far *src, struct Bitmap far *dst, int e);
void bm_opaque_span(struct Bitmap far *bm, int *left, int *right);
struct Bitmap far *bm_shift_right(struct Bitmap far *src, int n, struct Bitmap far *dst);
void bm_draw_masked(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag);
void bm_underlay(struct Bitmap far *src, struct Bitmap far *dst, int x, int y);
void bm_put(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode);
void bm_put_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y);
void bm_draw_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode);
void bm_and_mask(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in);
void bm_far_copy(char far *src, char far *dst, unsigned n);
struct Bitmap far *bm_grab(struct Bitmap far *bm, struct Blt *r, int mode);
void bm_copy_box(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2);

#endif
