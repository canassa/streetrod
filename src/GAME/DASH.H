/* DASH.H - declarations of what DASH.C defines. */

#ifndef DASH_H
#define DASH_H
#include "STRUCTS.H"

enum DashView {
    DASH_VIEW_SAVE = 0,
    DASH_VIEW_RESTORE = 1,
    DASH_VIEW_SHOW = 2,
    DASH_VIEW_DRIVE = 3,
    DASH_VIEW_RESTORE_SHOW = 4
};


struct ShifterDraw {
    struct Track *track;
    int knobPosition;
};

struct WheelPoses {
    int y[3];
    struct Bitmap far *bitmaps[3];
};

extern struct Hooks g_videoOps; /* DS:78A2 */

#define DASH_Y 100
#define DASH_HEIGHT 100

extern int g_needleColor;                               /* DS:02D2 */
extern char g_introPalette[];                                   /* DS:02D4 */
extern struct Bitmap far *g_savedBack;                  /* DS:02E6 */
extern struct Bitmap far *g_savedFront;                 /* DS:02EA */
extern signed char g_steerLeftPoints[24][2];                       /* DS:02EE */
extern unsigned char g_steerPoints[][2];                       /* DS:031E */
extern int g_wheelShown;                                /* DS:0350 */
extern int g_wheelSlotY[3];                             /* DS:0352 */
extern signed char g_shiftPts[11][2];                   /* DS:0358 */
extern struct Track g_shifters[];                       /* DS:036E */
extern struct Pair g_shiftSlots[];                      /* DS:038C */
extern struct Bitmap far *g_driveDisplayPage;                       /* DS:7678 */
extern unsigned g_targetPixelOffset;                                /* DS:767C */
extern unsigned g_videoAddrSegment;                                /* DS:787A */
extern char g_displayEnabled;                                     /* DS:8249 */
extern struct Bitmap far *g_bufferSceneA;                       /* DS:82B4 */
extern char g_runRacingFlag;                                     /* DS:8ACA */
void da_speedo_pts(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F4B7 */
void da_speedo_init(int i);                           /* 0x0F552 */
void da_tach_pts(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F62E */
void da_tach_init(int v);                             /* 0x0F6C9 */
int da_move_needle(int cur, int old, int flag);       /* 0x0F7A5 */
void da_draw_clock(void);                             /* 0x0F972 */
void far da_pal_blank(void);                          /* 0x0FEAD */
void far da_pal_show(void);                           /* 0x0FEBF */
void da_wait_key(int n);                              /* 0x0FED1 */
void da_intro(void);                                  /* 0x0FF70 */
void da_time_up(void);                                /* 0x100E2 */
void da_set_view(enum DashView n);                              /* 0x1013D */
void da_load_wheel(void);                             /* 0x102D3 */
void da_steer_marker(int a);                          /* 0x10414 */
void da_draw_wheel(int a, int b, struct Bitmap far *dst);/* 0x104B0 */
void da_init_shifter(int a, int b);                   /* 0x10677 */
void da_draw_shifter(struct Bitmap far *dst, struct Eng *e);/* 0x10872 */
void da_fmemset(char far *p, int c, unsigned n);      /* 0x109C0 */
int da_clip_blt(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out);/* 0x10A00 */
void da_retrace(int a);                                    /* 0x10AFD */
int da_text_width(char *s);                           /* 0x10B48 */

#endif
