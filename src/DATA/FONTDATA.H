/**
 * @file FONTDATA.H
 * @brief Glyph metrics and pixels for text rendering.
 */

#ifndef FONTDATA_H
#define FONTDATA_H

/**
 * @brief Variable-advance monochrome glyph for EGA and Tandy text.
 *
 * Records correspond to character codes 32 through 122, in ascending order.
 */
struct Glyph {
    unsigned char advance; /**< Text advance in pixels, including spacing. */
    /** Top-to-bottom rows; high bit is leftmost, set bits draw foreground. */
    char rows[8];
};

extern struct Glyph far g_font[];

#endif
