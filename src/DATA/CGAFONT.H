/* CGAFONT.H - declarations of what CGAFONT.ASM defines. */

#ifndef CGAFONT_H
#define CGAFONT_H

/* one glyph of the CGA text font (cg_draw_text; 91 glyphs for ' '..'z'): a width in half pixels and 8 rows of bits */
struct CgaGlyph {
    unsigned w;
    unsigned rows[8];
};

extern struct CgaGlyph far g_cgaFont[];

#endif
