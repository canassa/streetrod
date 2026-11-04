/* FONTDATA.H - declarations of what FONTDATA.ASM defines. */

#ifndef FONTDATA_H
#define FONTDATA_H

struct Glyph {
    unsigned char w;
    char rows[8];
};

extern struct Glyph far g_font[];

#endif
