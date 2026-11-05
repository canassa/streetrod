/**
 * @file EGABLIT.H
 * @brief Planar bitmap copies, masks and sprite composition.
 */

#ifndef EGABLIT_H
#define EGABLIT_H

struct Bitmap;
struct Blt;

/**
 * @brief Precomposed bottom sprite band copied through the EGA latches.
 *
 * Views the saved band supplied with a sprite frame; pixels reside in planar
 * video memory and rows are packed to the sprite's byte width.
 */
struct EgaSpriteBand {
    char far *data; /**< Borrowed first pixel byte in planar video memory. */
    int y;          /**< Source top row retained by the producer; unused here. */
    int h;          /**< Number of bottom rows to copy instead of composing. */
};

void eb_copy(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flags);
void eb_sprite(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct EgaSpriteBand *mask,
               struct Blt *r);
void eb_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag);
void eb_latch_pack(char far *src, int w, int h, char far *dst, int pitch);

#endif
