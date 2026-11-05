/**
 * @file BITMAP.H
 * @brief Bitmap storage and rectangle operations.
 */

#ifndef BITMAP_H
#define BITMAP_H
/** Header allocation size, including alignment. */
#define BITMAP_HEADER_BYTES 0x30

/**
 * @brief Bitmap storage kind used by the video drivers.
 *
 * Work bitmaps and decoded pictures use the active driver's pixel format.
 * Display pages use hardware planes or interleaved row banks.
 */
enum BitmapKind {
    BITMAP_WORK = -1,   /**< Linear work bitmap. */
    BITMAP_PICTURE = 1, /**< Linear decoded picture. */
    VID_EGA = -2,       /**< EGA page with hardware-selected color planes. */
    VID_CGA = -3,       /**< CGA page with two interleaved row banks. */
    VID_HERCULES = -4,  /**< Hercules display, drawn through the CGA driver. */
    VID_TANDY = -6      /**< Tandy page with four interleaved row banks. */
};


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

/**
 * @brief Pixel rectangle and optional transparency mask.
 *
 * Buffers can belong to the bitmap's allocation or be shared external views.
 * Linear pixel data holds consecutive rows and, on EGA, consecutive planes.
 */
struct Bitmap {
    int width;  /**< Width in pixels. */
    int height; /**< Height in pixel rows. */
    /** Pixel span in bytes; fixed EGA pages store one addressed plane's span. */
    unsigned pixelBytes;
    char unused[0x1e]; /**< Reserved bytes. */
    union FarPtr bits; /**< Color data, or zero for a mask-only image. */
    /** Optional transparency mask; sole data buffer for a mask-only image. */
    union FarPtr mask;
    int pitch;        /**< Bytes per pixel row in one plane or row bank. */
    signed char kind; /**< Storage selector from @ref BitmapKind. */
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
