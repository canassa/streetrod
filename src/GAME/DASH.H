/* DASH.H - declarations of what DASH.C defines. */

#ifndef DASH_H
#define DASH_H

#include "CURSOR.H"
#include "PERF.H"
#include "VIDEO.H"
struct Bitmap;
struct Blt;
struct DrivingState;
struct Sprite;

/* what g_shifterDraw.track points at (da_draw_shifter, da_init_shifter): number of points, bitmap ids of the two sprites, bitmap
   height at +6 and a table of (x, y) signed-byte pairs at +8 */
struct Track {
    int n;
    int id1;
    int id2;
    int h;
    signed char *pt;
};

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

extern struct Hooks g_videoOps;

#define DASH_Y 100
#define DASH_HEIGHT 100

extern struct Bitmap far *g_driveDisplayPage;
extern unsigned g_targetPixelOffset;
extern unsigned g_videoAddrSegment;
extern char g_displayEnabled;
extern struct Bitmap far *g_bufferSceneA;
extern char g_runRacingFlag;
void da_speedo_pts(int i, int *x1, int *y1, int *x2, int *y2);
void da_speedo_init(int i);
void da_tach_pts(int i, int *x1, int *y1, int *x2, int *y2);
void da_tach_init(int v);
int da_move_needle(int cur, int old, int flag);
void da_draw_clock(void);
void far da_pal_blank(void);
void far da_pal_show(void);
void da_wait_key(int n);
void da_intro(void);
void da_time_up(void);
void da_set_view(enum DashView n);
void da_load_wheel(void);
void da_steer_marker(int a);
void da_draw_wheel(int a, int b, struct Bitmap far *dst);
void da_init_shifter(enum AutoSelector a, enum DriveGear b);
void da_draw_shifter(struct Bitmap far *dst, struct DrivingState *e);
void da_fmemset(char far *p, int c, unsigned n);
int da_clip_blt(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out);
void da_retrace(int a);
int da_text_width(char *s);

#endif
