#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "DASH.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CRTC.H"
#include "CURSOR.H"
#include "DISK.H"
#include "FONTDATA.H"
#include "GAME.H"
#include "GARAGE.H"
#include "INPUTDAT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MOUSE.H"
#include "NEEDLE.H"
#include "PC.H"
#include "PERF.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "STRUCTS.H"
#include "SYS.H"
#include "VIDEO.H"
#include "VIEW.H"


enum DashSaveKind {
    DASH_SAVE_WHEEL = 1,
    DASH_SAVE_SPEEDO = 2,
    DASH_SAVE_TACH = 3
};
#define SPEEDO_CENTER_X 0xc4
#define TACH_CENTER_X 0x91
#define GAUGE_CENTER_Y 0x2a

static struct WheelPoses g_wheelPoses;
static void near save_under(int x, int y, int w, int h, enum DashSaveKind kind);
static void near restore_under(int x, int y, int w, int h, enum DashSaveKind kind);
static struct ShifterDraw g_shifterDraw;

struct Bitmap far *g_driveDisplayPage;
unsigned g_targetPixelOffset;
unsigned g_videoAddrSegment;
struct Hooks g_videoOps;
char g_displayEnabled;
struct Bitmap far *g_bufferSceneA;
char g_runRacingFlag;

static int g_needleColor = -1;
static char g_introPalette[18] = {0, 8, 7, 15, 14, 1, 9, 11, 0, 2, 4, 12, 13, 10, 5, 6, 0, 0};
static struct Bitmap far *g_savedBack = 0;
static struct Bitmap far *g_savedFront = 0;
static signed char g_steerLeftPoints[24][2] = {{87, 69},  {88, 64},  {90, 58},  {91, 54},   {93, 49},   {95, 45},
                                               {98, 40},  {101, 36}, {104, 33}, {108, 29},  {111, 26},  {114, 23},
                                               {118, 20}, {121, 19}, {125, 16}, {-127, 14}, {-122, 12}, {-117, 11},
                                               {-113, 9}, {-108, 8}, {-104, 7}, {-99, 7},   {-95, 6},   {-91, 6}};
static unsigned char g_steerPoints[25][2] = {
    {170, 6},  {175, 6},  {179, 6},  {183, 7},  {188, 7},  {192, 8},  {197, 9},  {201, 11}, {206, 12},
    {211, 14}, {215, 16}, {219, 19}, {222, 20}, {226, 23}, {229, 26}, {232, 29}, {236, 33}, {239, 36},
    {242, 40}, {245, 45}, {247, 49}, {249, 54}, {250, 58}, {252, 64}, {253, 69}};
static int g_wheelShown = 0;
static int g_wheelSlotY[3] = {148, 174, 73};
static signed char g_shiftPts[11][2] = {{14, 17}, {14, 28}, {13, 12}, {13, 2}, {13, 20}, {22, 2},
                                        {13, 12}, {13, 2},  {13, 20}, {22, 2}, {22, 20}};
static struct Track g_shifters[3] = {{2, PIC_SHIFTER_AUTOMATIC_GATE, PIC_SHIFTER_AUTOMATIC_KNOB, 8, g_shiftPts[0]},
                                     {4, PIC_SHIFTER_THREE_SPEED_GATE, PIC_SHIFTER_THREE_SPEED_KNOB, 19, g_shiftPts[2]},
                                     {5, PIC_SHIFTER_FOUR_SPEED_GATE, PIC_SHIFTER_FOUR_SPEED_KNOB, 19, g_shiftPts[6]}};
static struct Pair g_shiftSlots[5] = {{0, 162}, {32, 162}, {64, 162}, {96, 162}, {264, 102}};


static void near restore_under(int x, int y, int w, int h, enum DashSaveKind kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = 0x108;
    pkt.sy = (r_vid.mode == VID_EGA ? DASH_Y : 0) + (kind == DASH_SAVE_WHEEL ? 5 : kind == DASH_SAVE_SPEEDO ? 12 : 35);
    pkt.dx = x & 0xfff8;
    pkt.dy = y;
    (*g_videoOps.copy)(r_vid.mode == VID_EGA ? g_driveDisplayPage : g_bufferSceneA, g_driveDisplayPage, &pkt, 1);
}


static void near save_under(int x, int y, int w, int h, enum DashSaveKind kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = x & 0xfff8;
    pkt.sy = y;
    pkt.dx = 0x108;
    pkt.dy = (r_vid.mode == VID_EGA ? DASH_Y : 0) + (kind == DASH_SAVE_WHEEL ? 5 : kind == DASH_SAVE_SPEEDO ? 12 : 35);
    (*g_videoOps.copy)(g_dashBuffered ? g_bufBackgroundDashPtr : g_driveDisplayPage,
                       r_vid.mode == VID_EGA ? g_driveDisplayPage : g_bufferSceneA, &pkt, 1);
}


void da_speedo_pts(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > NEEDLE_MAX_POS)
        i = NEEDLE_MAX_POS;
    *x1 = g_needlePts[NEEDLE_TIP][i].x + SPEEDO_CENTER_X;
    *y1 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - g_needlePts[NEEDLE_TIP][i].y + GAUGE_CENTER_Y;
    *x2 = g_needlePts[NEEDLE_HUB][i].x + SPEEDO_CENTER_X;
    *y2 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - g_needlePts[NEEDLE_HUB][i].y + GAUGE_CENTER_Y;
    /* PROVISIONAL: the original CONST holds two more SEG constants for the needle segment
       (DS:68E6 with this function's, DS:68E8 later) that no instruction reads: MSC leaves
       them behind for unreachable far accesses to two other far symbols. The statements and
       symbol names are not recoverable; these keep the bytes and the relocation order. The
       block-scope extern is part of that provisional text (the symbols are labels in
       NEEDLE.ASM; see docs/RECONSTRUCTION.md). */
    return;
    {
        extern char far x_trk0[][2];
        *x1 = x_trk0[i][0];
    }
}


void da_speedo_init(int i)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) ? 11 : 3;
    da_speedo_pts(i, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    save_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_SPEEDO);
    if (g_dashBuffered == 0) {
        saveSeg = g_videoAddrSegment;
        saveOff = g_targetPixelOffset;
        g_videoAddrSegment = r_vid.mode == VID_EGA        ? EGA_SEGMENT
                             : r_vid.mode == VID_HERCULES ? MONO_SEGMENT
                                                          : CGA_SEGMENT;
        g_targetPixelOffset = 0;
        (*g_videoOps.drawLine)(x11, y1, x2, y2, color);
        g_videoAddrSegment = saveSeg;
        g_targetPixelOffset = saveOff;
    }
}


void da_tach_pts(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > NEEDLE_MAX_POS)
        i = NEEDLE_MAX_POS;
    *x1 = g_needlePts[NEEDLE_TIP][i].x + TACH_CENTER_X;
    *y1 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - g_needlePts[NEEDLE_TIP][i].y + GAUGE_CENTER_Y;
    *x2 = g_needlePts[NEEDLE_HUB][i].x + TACH_CENTER_X;
    *y2 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - g_needlePts[NEEDLE_HUB][i].y + GAUGE_CENTER_Y;
    /* PROVISIONAL: second unreachable far access, see da_speedo_pts. */
    return;
    {
        extern char far x_trk1[][2];
        *x2 = x_trk1[i][0];
    }
}


void da_tach_init(int v)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = r_vid.mode == VID_EGA ? 11 : 3;
    da_tach_pts(v / 0x2b, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    save_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_TACH);
    if (g_dashBuffered == 0) {
        saveSeg = g_videoAddrSegment;
        saveOff = g_targetPixelOffset;
        g_videoAddrSegment = r_vid.mode == VID_EGA        ? EGA_SEGMENT
                             : r_vid.mode == VID_HERCULES ? MONO_SEGMENT
                                                          : CGA_SEGMENT;
        g_targetPixelOffset = 0;
        (*g_videoOps.drawLine)(x11, y1, x2, y2, color);
        g_videoAddrSegment = saveSeg;
        g_targetPixelOffset = saveOff;
    }
}


int da_move_needle(int cur, int old, int flag)
{
    int miny3, minx4, y1, x2, x11, y2;
    unsigned saveOff, saveSeg;

    if (g_needleColor < 0)
        g_needleColor = (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) ? 11 : 3;
    saveSeg = g_videoAddrSegment;
    saveOff = g_targetPixelOffset;
    g_videoAddrSegment = r_vid.mode == VID_EGA ? EGA_SEGMENT : r_vid.mode == VID_HERCULES ? MONO_SEGMENT : CGA_SEGMENT;
    g_targetPixelOffset = 0;
    if (flag) {
        da_tach_pts(old / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        restore_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_TACH);
    } else {
        da_speedo_pts(old, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        restore_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_SPEEDO);
    }
    if (flag) {
        da_tach_pts(cur / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        save_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_TACH);
    } else {
        da_speedo_pts(cur, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        save_under(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, DASH_SAVE_SPEEDO);
    }
    (*g_videoOps.drawLine)(x11, y1, x2, y2, g_needleColor);
    g_videoAddrSegment = saveSeg;
    g_targetPixelOffset = saveOff;
    return cur;
}

/* Draws the clock: the hour hand (width 5 or 8) and the minute hand (width 4 or 8), each a
   diamond of five lines around the face centre, from the time ga_time returns. */

void da_draw_clock(void)
{
    int a2, x2, y2, x3, x4;
    double pi3;
    unsigned soffn;
    int h4;
    double rad5;
    int y3, m, w6;
    unsigned ssegk;
    int y13, al, x14, y4, rk;

    pi3 = 3.1415926535;
    if (g_needleColor < 0)
        g_needleColor = (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) ? 11 : 3;
    ssegk = g_videoAddrSegment;
    soffn = g_targetPixelOffset;
    g_videoAddrSegment = r_vid.mode == VID_EGA ? EGA_SEGMENT : r_vid.mode == VID_HERCULES ? MONO_SEGMENT : CGA_SEGMENT;
    g_targetPixelOffset = 0;
    ga_time(&h4, &m);
    h4 >>= 1;
    h4 += 0x14;
    h4 = h4 % 12 + 1;
    al = 0x5a - (30 * h4 + (m >> 1));
    rk = abs(al - al / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 5;
    rad5 = al * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 13.0) + 0x91;
    y2 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin(rad5) * 13.0) + 0x2a;
    x3 = (int)(cos((al + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin((al + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((al - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin((al - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_videoOps.drawLine)(x14, y13, x2, y2, g_needleColor);
    (*g_videoOps.drawLine)(x14, y13, x3, y3, g_needleColor);
    (*g_videoOps.drawLine)(x14, y13, x4, y4, g_needleColor);
    (*g_videoOps.drawLine)(x2, y2, x3, y3, g_needleColor);
    (*g_videoOps.drawLine)(x2, y2, x4, y4, g_needleColor);
    a2 = 0x5a - 6 * m;
    rk = abs(a2 - a2 / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 4;
    rad5 = a2 * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 18.0) + 0x91;
    y2 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin(rad5) * 18.0) + 0x2a;
    x3 = (int)(cos((a2 + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin((a2 + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((a2 - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) - (int)(sin((a2 - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_videoOps.drawLine)(x14, y13, x2, y2, g_needleColor);
    (*g_videoOps.drawLine)(x14, y13, x3, y3, g_needleColor);
    (*g_videoOps.drawLine)(x14, y13, x4, y4, g_needleColor);
    (*g_videoOps.drawLine)(x2, y2, x3, y3, g_needleColor);
    (*g_videoOps.drawLine)(x2, y2, x4, y4, g_needleColor);
    g_videoAddrSegment = ssegk;
    g_targetPixelOffset = soffn;
}


void da_pal_blank(void)
{
    vi_set_palette(g_blackPalette);
    g_displayEnabled = 0;
}


void da_pal_show(void)
{
    vi_set_palette(g_palette);
    g_displayEnabled = 1;
}


void da_wait_key(int ticks)
{
    long end;
    char key;

    end = g_tick + ticks;
    while (end > g_tick) {
        if (g_click) {
            g_click = 0;
            break;
        }
        if (g_mouse.present) {
            mo_buttons();
            if (g_mouse.left)
                break;
        }
        if (g_joystick) {
            jy_read();
            if (g_joyButtons)
                break;
        }
        key = kg_key();
        if (key == 'M') {
            if (g_musicOn) {
                sy_music_pause();
                g_musicOn = 0;
            } else {
                g_musicOn = 1;
                sy_music_resume();
            }
        } else if (key == ASCII_ENTER || key == ASCII_ESCAPE)
            break;
    }
}


void da_intro(void)
{
    int saved;

    saved = g_picturePlacement.maxHeight;
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY)
        (*g_videoOps.blank)();
    g_picturePlacement.maxHeight = SCREEN_HEIGHT;
    if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY)
        mi_set_palette(CGA_PALETTE_GREEN_RED_BROWN);
    sc_picture_at(PIC_PUBLISHER_LOGO, 0x28, 0x28, 0);
    di_free_pic(PIC_PUBLISHER_LOGO);
    (*g_videoOps.show)();
    r_vid.copyToFront = 0;
    sc_picture(PIC_TITLE, -1);
    di_free_pic(PIC_TITLE);
    da_wait_key(400);
    (*g_videoOps.show)();
    if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY)
        mi_set_palette(CGA_PALETTE_CYAN_MAGENTA_WHITE);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
    sc_picture(PIC_CREDITS_LICENSE_PLATE, 0);
    di_free_pic(PIC_CREDITS_LICENSE_PLATE);
    da_wait_key(400);
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY)
        vi_set_palette(g_introPalette);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
    da_wait_key(400);
    g_picturePlacement.maxHeight = saved;
    sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
}


void da_time_up(void)
{
    gm_message(0x17bb);
    (*g_videoOps.blank)();
    cu_ctl(CURSOR_HIDE_NOW);
    r_vid.copyToFront = 1;
    sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    ga_calendar(1);
    vi_garage_pal();
    (*g_videoOps.show)();
    gm_pause(300);
}


void da_set_view(enum DashView mode)
{
    switch (mode) {
    case DASH_VIEW_SAVE:
        if (!g_savedFront) {
            g_savedFront = r_vid.front;
            g_savedBack = r_vid.back;
        }
        g_dashViewSaved = 1;
        break;
    case DASH_VIEW_RESTORE:
        r_vid.front = g_savedFront;
        r_vid.back = g_savedBack;
        if (r_vid.mode == VID_EGA) {
            outp(CRTC_INDEX, CRTC_OFFSET);
            outp(CRTC_DATA, 0x14);
            ct_set_split(-1);
            outp(CRTC_INDEX, CRTC_START_HI);
            outp(CRTC_DATA, 0);
            outp(CRTC_INDEX, CRTC_START_LO);
            outp(CRTC_DATA, 0);
        }
        r_vid.copyToFront = 1;
        sc_status_init(SCREEN_BODY_HEIGHT);
        break;
    case DASH_VIEW_SHOW:
        if (r_vid.mode == VID_EGA) {
            outp(CRTC_INDEX, CRTC_START_HI);
            outp(CRTC_DATA, 0);
            outp(CRTC_INDEX, CRTC_START_LO);
            outp(CRTC_DATA, 0);
        }
        r_vid.copyToFront = 1;
        break;
    case DASH_VIEW_DRIVE:
        r_vid.back = r_vid.front = g_savedFront;
        r_vid.copyToFront = 0;
        sc_status_init(r_vid.mode == VID_EGA ? 0x5a : SCREEN_BODY_HEIGHT);
        if (g_runRacingFlag)
            sc_label(g_gearLabel);
        break;
    case DASH_VIEW_RESTORE_SHOW:
        r_vid.front = g_savedFront;
        r_vid.back = g_savedBack;
        if (r_vid.mode == VID_EGA) {
            outp(CRTC_INDEX, CRTC_OFFSET);
            outp(CRTC_DATA, 0x14);
            ct_set_split(-1);
            outp(CRTC_INDEX, CRTC_START_HI);
            outp(CRTC_DATA, 0);
            outp(CRTC_INDEX, CRTC_START_LO);
            outp(CRTC_DATA, 0);
        }
        r_vid.copyToFront = 1;
        (*g_videoOps.show)();
        break;
    }
}


void da_load_wheel(void)
{
    int i6;
    struct Bitmap far *bm;
    struct Blt pkt;

    if (r_vid.mode != VID_EGA) {
        pkt.w = 0xb8;
        pkt.h = 0x1a;
        pkt.sx = pkt.sy = 0;
        pkt.dx = 0x88;
        for (i6 = 0; i6 < 3; i6++) {
            bm = di_bm(g_wheelPicIds[i6], ARENA_LOW_STACK);
            if (r_vid.mode != VID_EGA || i6 < 2) {
                g_wheelPoses.bitmaps[i6] = g_bufferSceneA;
                g_wheelPoses.y[i6] = g_wheelSlotY[i6];
            } else {
                g_wheelPoses.bitmaps[i6] = g_driveDisplayPage;
                g_wheelPoses.y[i6] = g_wheelSlotY[i6] + 0x66;
            }
            pkt.dy = g_wheelPoses.y[i6];
            (*g_videoOps.copy)(bm, g_wheelPoses.bitmaps[i6], &pkt, 1);
            ma_pop_low(1);
        }
    } else {
        for (i6 = 0; i6 < 3; i6++)
            g_wheelPoses.bitmaps[i6] = di_bm(g_wheelPicIds[i6], ARENA_LOW_STACK);
    }
}


void da_steer_marker(int n)
{
    int x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    seg = g_videoAddrSegment;
    off = g_targetPixelOffset;
    g_videoAddrSegment = r_vid.mode == VID_EGA        ? EGA_PAGE1_SEGMENT
                         : r_vid.mode == VID_HERCULES ? MONO_SEGMENT
                                                      : CGA_SEGMENT;
    g_targetPixelOffset = 0;
    vw_unclip_top();
    x = g_steerPoints[n][0];
    y2 = g_steerPoints[n][1] + DASH_Y;
    ymax = g_viewClip.yMax;
    ymin2 = g_viewClip.yMin;
    g_viewClip.yMax = SCREEN_BODY_HEIGHT;
    g_viewClip.yMin = DASH_Y;
    vw_draw_dot(x, y2);
    g_viewClip.yMax = ymax;
    g_viewClip.yMin = ymin2;
    vw_clip();
    g_videoAddrSegment = seg;
    g_targetPixelOffset = off;
}


void da_draw_wheel(int a, int b, struct Bitmap far *dst)
{
    struct Blt pkt1;
    int sel, x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    sel = (a != 0) + (a > 0);
    if (sel < 3 && sel != g_wheelShown) {
        pkt1.w = 0xb8;
        pkt1.h = 0x1a;
        if (r_vid.mode != VID_EGA) {
            pkt1.sx = 0x88;
            pkt1.sy = g_wheelPoses.y[sel];
        } else
            pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x50;
        pkt1.dy = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) + 0x3f;
        (*g_videoOps.copy)(g_wheelPoses.bitmaps[sel], dst, &pkt1, 1);
        g_wheelShown = sel;
    }
    if (b != 100) {
        x = g_steerPoints[b][0];
        y2 = (r_vid.mode == VID_EGA ? 0 : DASH_Y) + g_steerPoints[b][1];
        restore_under(x - 2, y2 - 2, 5, 5, DASH_SAVE_WHEEL);
    }
    x = g_steerPoints[a][0];
    y2 = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) + g_steerPoints[a][1];
    save_under(x - 2, y2 - 2, 5, 5, DASH_SAVE_WHEEL);
    if (g_dashBuffered == 0) {
        seg = g_videoAddrSegment;
        off = g_targetPixelOffset;
        g_videoAddrSegment = r_vid.mode == VID_EGA        ? EGA_SEGMENT
                             : r_vid.mode == VID_HERCULES ? MONO_SEGMENT
                                                          : CGA_SEGMENT;
        g_targetPixelOffset = 0;
        vw_unclip_top();
        if (r_vid.mode != VID_EGA) {
            ymax = g_viewClip.yMax;
            ymin2 = g_viewClip.yMin;
            g_viewClip.yMax = SCREEN_BODY_HEIGHT;
            g_viewClip.yMin = DASH_Y;
        }
        vw_draw_dot(x, y2);
        if (r_vid.mode != VID_EGA) {
            g_viewClip.yMax = ymax;
            g_viewClip.yMin = ymin2;
        }
        vw_clip();
        g_videoAddrSegment = seg;
        g_targetPixelOffset = off;
    }
}

/* Draws the car sprite at the top of the screen and, for each of the g_shifterDraw.track->n points of the
   track, the wheel sprite (the position comes from the (x, y) pair, the source rectangle from
   g_shiftSlots). da_draw_shifter is the per-frame update that draws the same pieces. */

void da_init_shifter(int a, int b)
{
    struct Blt pk5;
    struct Bitmap far *cm2;
    struct Blt p2;
    struct Blt p3;
    int x;
    struct Bitmap far *bm19;
    int i;
    struct Bitmap far *bm28;
    int y;

    g_shifterDraw.track = &g_shifters[a ? 0 : b == 3 ? 1 : 2];
    bm19 = di_bm(g_shifterDraw.track->id1, ARENA_HIGH_STACK);
    pk5.w = 0x30;
    pk5.h = 0x2b;
    pk5.sx = pk5.sy = 0;
    pk5.dx = 0x108;
    pk5.dy = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) + 0x17;
    (*g_videoOps.copy)(bm19, g_dashBuffered ? g_bufBackgroundDashPtr : g_driveDisplayPage, &pk5, 1);
    bm28 = di_bm(g_shifterDraw.track->id2, ARENA_HIGH_STACK);
    p3.w = p2.w = (pk5.w = bm28->width) + 8;
    p3.h = p2.h = pk5.h = bm28->height;
    p3.sx = p3.sy = 0;
    p2.dx = p2.dy = 0;
    pk5.sx = pk5.sy = pk5.dy = 0;
    for (i = 0; i < g_shifterDraw.track->n; i++) {
        x = g_shifterDraw.track->pt[i * 2];
        y = g_shifterDraw.track->pt[i * 2 + 1];
        p2.sx = x & 0xfff8;
        p2.sy = y;
        cm2 = bm_grab(bm19, &p2, ARENA_LOW_STACK);
        p3.dx = g_shiftSlots[i].a;
        p3.dy = g_shiftSlots[i].b;
        (*g_videoOps.copy)(cm2, g_bufferSceneA, &p3, 1);
        pk5.dx = x - p2.sx;
        bm_draw_masked(bm28, cm2, &pk5, 0);
        p3.dy += 0x13;
        (*g_videoOps.copy)(cm2, g_bufferSceneA, &p3, 1);
        ma_pop_low(1);
    }
    g_shifterDraw.knobPosition = -1;
    ma_pop_high(2);
}


void da_draw_shifter(struct Bitmap far *dst, struct Eng *e)
{
    struct Blt16 pkt1;
    int x, idx3;

    if (e->autoSelector == SELECTOR_MANUAL)
        idx3 = e->gearIndex;
    else if (e->gearIndex == 0)
        idx3 = 0;
    else
        idx3 = e->autoSelector - 1;
    pkt1.h = g_shifterDraw.track->h;
    if (g_shifterDraw.knobPosition >= 0) {
        pkt1.sx = g_shiftSlots[g_shifterDraw.knobPosition].a;
        pkt1.sy = g_shiftSlots[g_shifterDraw.knobPosition].b;
        x = g_shifterDraw.track->pt[g_shifterDraw.knobPosition * 2] & 0xfff8;
        pkt1.dx = x + 0x108;
        pkt1.dy = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) +
                  g_shifterDraw.track->pt[g_shifterDraw.knobPosition * 2 + 1] + 0x17;
        pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
        (*g_videoOps.copy)(g_bufferSceneA, dst, (struct Blt *)&pkt1, 1);
    }
    pkt1.sx = g_shiftSlots[idx3].a;
    pkt1.sy = g_shiftSlots[idx3].b + 0x13;
    x = g_shifterDraw.track->pt[idx3 * 2] & 0xfff8;
    pkt1.dx = x + 0x108;
    pkt1.dy = (r_vid.mode == VID_EGA || g_dashBuffered ? 0 : DASH_Y) + g_shifterDraw.track->pt[idx3 * 2 + 1] + 0x17;
    pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
    (*g_videoOps.copy)(g_bufferSceneA, dst, (struct Blt *)&pkt1, 1);
    g_shifterDraw.knobPosition = idx3;
    e->shifterDirty = 0;
}


void da_fmemset(char far *p, int c, unsigned n)
{
    char far *q;

    for (q = p; n; n--)
        *q++ = c;
}

/* Clips the rectangle in to the destination bitmap, then to the
   source bitmap; the result goes to out. Returns 1 if anything is left. The x values of in
   are shifted down by r_vid.shift. */

int da_clip_blt(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out)
{
    int t1, t2;
    int w1, h;
    int sx, sy, dx, dy;

    sx = in->sx >> r_vid.shift;
    sy = in->sy;
    w1 = in->w >> r_vid.shift;
    h = in->h;
    if (sx + w1 > dst->pitch)
        w1 = dst->pitch - sx;
    if (sy + h > dst->height)
        h = dst->height - sy;
    dx = in->dx >> r_vid.shift;
    dy = in->dy;
    if (dy < 0) {
        h += dy;
        sy -= dy;
        dy = 0;
    }
    if ((t1 = src->height) < dy + h)
        h = t1 - dy;
    if (dx < 0) {
        w1 += dx;
        sx -= dx;
        dx = 0;
    }
    if ((t2 = src->pitch) < dx + w1)
        w1 = t2 - dx;
    out->sx = sx;
    out->sy = sy;
    out->dx = dx;
    out->dy = dy;
    out->w = w1;
    out->h = h;
    return out->w > 0 && out->h > 0;
}


void da_retrace(int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (r_vid.mode != VID_HERCULES) {
            while (inp(INPUT_STATUS) & STATUS_VRETRACE)
                ;
            while (!(inp(INPUT_STATUS) & STATUS_VRETRACE))
                ;
        } else {
            while (!(inp(MONO_STATUS) & HERC_STATUS_VSYNC))
                ;
            while (inp(MONO_STATUS) & HERC_STATUS_VSYNC)
                ;
        }
    }
}


int da_text_width(char *s)
{
    int w;

    for (w = 0; *s; s++)
        w += g_font[*s - 32].w;
    return w;
}
