/* CARHOP.H - declarations of what CARHOP.C defines. */

#ifndef CARHOP_H
#define CARHOP_H

#include "SPRITE.H"

struct Sheet {
    int id;
    int cols;
    int n;
};

enum CarhopMode {
    CARHOP_ENTER = 0,
    CARHOP_LEAVE = 1
};

extern struct Sheet g_carhopSheets[];
extern char g_carhopFrames[];
extern struct Seq g_carhopInSeq[4];
extern struct Seq g_carhopOutSeq[4];
extern char g_keepMask;
void ch_skate(enum CarhopMode a, int b);
void ch_save_bg(void);

#endif
