/* CARPIC.H - declarations of what CARPIC.C defines. */

#ifndef CARPIC_H
#define CARPIC_H

#include "BITMAP.H"

/* one saved band of a screen page (8 bytes): where it sits in the scratch bitmap, its first row and height */
struct Band {
    char far *p;
    int y;
    int h;
};

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

extern int g_delayScale;
extern int g_dashBuffered;
extern int g_autoTrans;
extern int g_wheelPicIds[3];
extern int g_driveRebuild;
extern struct Cmd g_animScripts[];
extern int g_animScriptStart[];
extern int g_carMoving;
extern int g_backdropX;
extern int g_carPassBy;
extern struct Bitmap g_bandRecordsSprArray[];
extern struct Bitmap far *g_lastPic;
extern struct Band g_carSpriteStrips[4];
extern struct Bitmap g_carBackdropHdrBmp;
extern int g_carVisibleHtValue;
void cp_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb, int y1, int y0,
                 struct Rect **pr1, struct Rect **pr2);
void cp_build(struct Bitmap far **out, int idx, int flags, int kk, int q, int *ids, int w, int n);
void cp_show_car(enum CarAnimKind kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids);
void cp_patch_bg(struct Bitmap far *bm, int a, int b, int c, int d, int e, int f, int g, int h);

#endif
