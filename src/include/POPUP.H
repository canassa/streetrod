/* POPUP.H - declarations of what POPUP.C defines. */

#ifndef POPUP_H
#define POPUP_H
#include "STRUCTS.H"

extern struct Win g_04c4[];                             /* DS:04C4 */
extern struct Bitmap far *g_04fa;                       /* DS:04FA */
struct Bitmap far *F_11A72(int x, int y, int w, int h, struct Bitmap far *src);/* 0x11A72 */
void F_11B6F(struct Bitmap far *bm, int x, int y);      /* 0x11B6F */
void F_11BD3(struct Bitmap far *bm, int x, int y);      /* 0x11BD3 */
void F_11C24(int n);                                    /* 0x11C24 */
void F_11DE2(int n, int x, int y);                      /* 0x11DE2 */
void F_11E5F(void);                                     /* 0x11E5F */

#endif
