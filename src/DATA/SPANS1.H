/* SPANS1.H - declarations of what SPANS1.ASM defines. */

#ifndef SPANS1_H
#define SPANS1_H

struct Part {
    signed char dx;
    signed char dy;
    signed char w;
    signed char color;
};

extern struct Part far g_spans1[];

#endif
