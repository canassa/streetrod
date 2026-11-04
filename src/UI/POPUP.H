/* POPUP.H - declarations of what POPUP.C defines. */

#ifndef POPUP_H
#define POPUP_H
#include "STRUCTS.H"

extern struct Win g_popupWins[];                        /* DS:04C4 */
extern struct Bitmap far *g_popupSave;                  /* DS:04FA */
struct Bitmap far *pu_grab(int x, int y, int w, int h, struct Bitmap far *src);/* 0x11A72 */
void pu_put(struct Bitmap far *bm, int x, int y);    /* 0x11B6F */
void pu_show(struct Bitmap far *bm, int x, int y);   /* 0x11BD3 */
void pu_open(int n);                                 /* 0x11C24 */
void pu_open_at(int n, int x, int y);                /* 0x11DE2 */
void pu_close(void);                                 /* 0x11E5F */

#endif
