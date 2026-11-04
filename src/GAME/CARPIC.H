/* CARPIC.H - declarations of what CARPIC.C defines. */

#ifndef CARPIC_H
#define CARPIC_H
#include "STRUCTS.H"

#define BACKDROP_ROWS 70

enum CarAnimKind {
    CAR_DRIVE_IN = 1,
    CAR_DRIVE_OUT = 2,
    CAR_SHOW_BACK_PAGE = 3,
    CAR_PULL_IN = 4,
    CAR_BACK_OUT = 6,
    CAR_SHOW_SCREEN = 7,
    CAR_SHOW_BACKDROP = 8
};

extern int g_delayScale;                                /* DS:58E2 */
extern int g_dashBuffered;                                      /* DS:58E4 */
extern int g_autoTrans;                                 /* DS:58E6 */
extern int g_wheelPicIds[3];                            /* DS:58E8 */
extern int g_driveRebuild;                                      /* DS:58EE */
extern struct Cmd g_animScripts[];                      /* DS:58F0 */
extern int g_animScriptStart[];                         /* DS:59F8 */
extern struct Bitmap far *g_backdrop;                   /* DS:5A0A */
extern int g_carMoving;                                 /* DS:5A0E */
extern struct Rect g_overlayRectA;                              /* DS:5A10 */
extern struct Rect g_overlayRectB;                              /* DS:5A18 */
extern int g_backdropX;                                 /* DS:5A20 */
extern int g_backdropW;                                 /* DS:5A22 */
extern int g_carPassBy;                                 /* DS:5A24 */
extern struct Bitmap g_bandRecordsSprArray[];                         /* DS:6E68 */
extern struct Bitmap far *g_lastPic;                       /* DS:70A8 */
extern struct Band g_carSpriteStrips[4];                           /* DS:7EC2 */
extern struct Bitmap g_carBackdropHdrBmp;                            /* DS:7FA8 */
extern int g_carVisibleHtValue;                                      /* DS:8262 */
void cp_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb, int y1, int y0, struct Rect **pr1, struct Rect **pr2);/* 0x1783E */
void cp_build(struct Bitmap far **out, int idx, int flags, int kk, int q, int *ids, int w, int n);/* 0x17BE6 */
void cp_show_car(enum CarAnimKind kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids);/* 0x181C8 */
void cp_patch_bg(struct Bitmap far *bm, int a, int b, int c, int d, int e, int f, int g, int h);/* 0x18ADB */

#endif
