/* CARPIC.H - declarations of what CARPIC.C defines. */

#ifndef CARPIC_H
#define CARPIC_H
#include "STRUCTS.H"

extern int g_delayScale;                                /* DS:58E2 */
extern int g_58e4;                                      /* DS:58E4 */
extern int g_autoTrans;                                 /* DS:58E6 */
extern int g_wheelPicIds[3];                            /* DS:58E8 */
extern int g_58ee;                                      /* DS:58EE */
extern struct Cmd g_animScripts[];                      /* DS:58F0 */
extern int g_animScriptStart[];                         /* DS:59F8 */
extern struct Bitmap far *g_backdrop;                   /* DS:5A0A */
extern int g_carMoving;                                 /* DS:5A0E */
extern struct Rect g_5a10;                              /* DS:5A10 */
extern struct Rect g_5a18;                              /* DS:5A18 */
extern int g_backdropX;                                 /* DS:5A20 */
extern int g_backdropW;                                 /* DS:5A22 */
extern int g_carPassBy;                                 /* DS:5A24 */
extern struct Bitmap am6e68x[];                         /* DS:6E68 */
extern struct Bitmap far *at70a8;                       /* DS:70A8 */
extern int (far *g78c6c)(struct Sprite *);              /* DS:78C6 */
extern struct Band dz7ec2[4];                           /* DS:7EC2 */
extern struct Bitmap bw7fa8;                            /* DS:7FA8 */
extern int l_8262;                                      /* DS:8262 */
void carpic_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb, int y1, int y0, struct Rect **pr1, struct Rect **pr2);/* 0x1783E */
void carpic_build(struct Bitmap far **out, int idx, int flags, int kk, int q, int *ids, int w, int n);/* 0x17BE6 */
void F_181C8(int kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids);/* 0x181C8 */
void carpic_patch_bg(struct Bitmap far *bm, int a, int b, int c, int d, int e, int f, int g, int h);/* 0x18ADB */

#endif
