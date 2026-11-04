/* NEEDLE.H - declarations of what NEEDLE.ASM defines. */

#ifndef NEEDLE_H
#define NEEDLE_H

struct NeedlePoint {
    signed char x, y;
};

enum NeedleEnd {
    NEEDLE_TIP,
    NEEDLE_HUB
};

#define NEEDLE_MAX_POS 0x82

extern struct NeedlePoint far g_needlePts[2][NEEDLE_MAX_POS + 1];

#endif
