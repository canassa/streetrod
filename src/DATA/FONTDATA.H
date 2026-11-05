/**
 * @file FONTDATA.H
 * @brief Glyph metrics and pixels for text rendering.
 */

#ifndef FONTDATA_H
#define FONTDATA_H

struct Glyph {
    unsigned char w;
    char rows[8];
};

extern struct Glyph far g_font[];

#endif
