/* CARHOP.H - declarations of what CARHOP.C defines. */

#ifndef CARHOP_H
#define CARHOP_H
#include "STRUCTS.H"

#define CARHOP_ENTER 0
#define CARHOP_LEAVE 1

extern struct Sheet g_carhopSheets[];
extern char g_carhopFrames[];
extern struct Seq g_carhopInSeq[4];
extern struct Seq g_carhopOutSeq[4];
extern char g_keepMask;
void ch_skate(int a, int b);
void ch_save_bg(void);

#endif
