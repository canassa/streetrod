/**
 * @file DASH.H
 * @brief Dashboard gauges, shifter and display pages.
 */

#ifndef DASH_H
#define DASH_H

#include "CURSOR.H"
#include "PERF.H"
#include "VIDEO.H"
struct Bitmap;
struct Blt;
struct DrivingState;
struct Sprite;

/** @brief Shifter gate artwork and knob coordinates for one transmission. */
struct ShifterTrack {
    int positionCount;      /**< Number of available knob positions. */
    int gatePic;            /**< Gate-background picture ID. */
    int knobPic;            /**< Knob picture ID. */
    int knobHeight;         /**< Knob-patch copy height in pixels. */
    signed char *positions; /**< Interleaved X,Y pixels relative to the gate. */
};

/** No knob patch has been drawn since shifter initialization. */
#define SHIFTER_NOT_DRAWN (-1)

/** @brief Steering-wheel artwork slots. */
enum WheelPose {
    WHEEL_POSE_CENTER = 0, /**< Straight-ahead wheel. */
    WHEEL_POSE_LEFT = 1,   /**< Wheel turned left. */
    WHEEL_POSE_RIGHT = 2,  /**< Wheel turned right. */
    WHEEL_POSE_COUNT = 3   /**< Number of wheel pictures. */
};

/** @brief Page pointers and hardware layout for scene transitions. */
enum DashView {
    DASH_VIEW_SAVE = 0, /**< Remember the original front and back pages once. */
    /** Restore saved pages, full-screen layout and status line. */
    DASH_VIEW_RESTORE = 1,
    /** Enable front-page copying and reset the EGA display start. */
    DASH_VIEW_SHOW = 2,
    /** Share the saved front page for driving; disable front-page copying. */
    DASH_VIEW_DRIVE = 3,
    /** Restore saved pages and full-screen layout, then show the page. */
    DASH_VIEW_RESTORE_SHOW = 4
};

/** @brief Active shifter layout and the knob patch that needs erasing. */
struct ShifterDraw {
    struct ShifterTrack *track; /**< Borrowed transmission layout. */
    /** Previous layout index, or @ref SHIFTER_NOT_DRAWN before drawing. */
    int knobPosition;
};

/** @brief Source images and rows indexed by @ref WheelPose. */
struct WheelPoses {
    /** Source rows for non-EGA wheel copies; unused on EGA. */
    int y[WHEEL_POSE_COUNT];
    /** Shared scene bitmap outside EGA; separate wheel images on EGA. */
    struct Bitmap far *bitmaps[WHEEL_POSE_COUNT];
};

extern struct Hooks g_videoOps;

/**
 * @name Dashboard screen band
 * @brief Top row and height in pixels; EGA displays the band with a
 * split-screen address.
 * @{
 */
#define DASH_Y 100
#define DASH_HEIGHT 100
/** @} */

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
