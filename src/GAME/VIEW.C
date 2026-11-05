#include <stdlib.h>
#include "VIEW.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CRUISE.H"
#include "DASH.H"
#include "DISK.H"
#include "DRIVE.H"
#include "EGAPAGE.H"
#include "EGASPAN.H"
#include "MAIN.H"
#include "MARKS.H"
#include "PC.H"
#include "RACE.H"
#include "ROAD.H"
#include "SCENERY.H"
#include "SHRINK.H"
#include "SLOTS.H"
#include "SYS.H"

enum QuadShape {
    QUAD_LEFT_ENCLOSES_RIGHT = 2,
    QUAD_RIGHT_ENCLOSES_LEFT = 3,
    QUAD_LEFT_HIGHER = 4,
    QUAD_RIGHT_HIGHER = 5
};

static int g_horizonY;
static int g_roadRowLimit;
static struct RenderRows g_renderRows;
static int g_prevRivalFrame;
static void near clip_blit_y(struct Blt *b);
static int near edge_step(int x1, int y1, int x2, int y2);
static int near sgn(int n);
static void near fill_quad(int mode, int a, int b, int c, int d, int e, int f);
static void near render_buildings(int, int, int, int, int, int, int);
static void near render_left_buildings(void);
static void near render_right_buildings(void);
static void near render_mirror_buildings(int, int, int, int, int, int, int);
static void near render_mirror_left(void);
static void near render_mirror_right(void);
static void near clip_blit(struct Blt *b);
static void near clip_vline(int color, int x, int y1, int y2);
static void near render_scaled_pic(int x, int y, int n, int align, int clip, int mode);
static void near render_object(int x, int y, int n, int f, enum SceneryId idx);
static void near clip_mirror(void);
static void near render_mirror_row(void);
static void near render_hydrant(int x, int y, int n, int f);
static void near render_gantry(int x1, int x2, int y1, int y2, int n, int clip, int flags);
static void near render_mirror_frame(void);
static void near render_opponent(void);
static void near flip_page(void);
static void near render_backdrop(void);
static void near render_row(int n);
static void near render_start_signal(void);
static void near render_overlays(void);
static void near render_mirror(void);
static void near select_draw_page(void);

static int g_opponentPics[5][4] = {
    {PIC_CORVETTE_PASSING, PIC_CORVETTE_REAR_ANGLED, PIC_CORVETTE_REAR, PIC_CORVETTE_DISTANT_REAR},
    {PIC_SEDAN_PASSING, PIC_SEDAN_REAR_ANGLED, PIC_SEDAN_REAR, PIC_SEDAN_SMALL_REAR},
    {PIC_SEDAN_PASSING, PIC_COUPE_REAR_ANGLED, PIC_COUPE_REAR, PIC_SEDAN_DISTANT_REAR},
    {PIC_SEDAN_PASSING, PIC_HOT_ROD_REAR_ANGLED, PIC_HOT_ROD_REAR, PIC_HOT_ROD_DISTANT_REAR},
    {PIC_SEDAN_PASSING, PIC_SPORTS_CAR_REAR_ANGLED, PIC_SPORTS_CAR_REAR, PIC_SPORTS_CAR_DISTANT_REAR}};
static int g_unused20 = 0;
struct ViewPages g_viewPages = {1, 2};
struct Clip g_viewRect = {SCREEN_WIDTH, 82, 0, 18, 0, 18};
struct ViewClip g_viewClip = {0, 18, 101, (SCREEN_WIDTH - 1)};
static unsigned char g_shrinkMasks[3][4] = {{87, 37, 34, 0}, {247, 117, 82, 0}, {117, 84, 33, 0}};
static int g_sceneBitmapIds[4] = {PIC_SIGN_COUNTY_LINE, PIC_SIGN_GARAGE, PIC_SIGN_BOBS, PIC_SIGN_GAS_STATION};
static int g_roadLineWidths[21] = {8, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int g_unused21[4] = {0, 0, 0, 0};
static struct Ball g_sheetRects[7] = {{256, 8, 8},   {240, 16, 13}, {208, 32, 19}, {168, 40, 24},
                                      {120, 48, 30}, {64, 56, 37},  {0, 64, 43}};


static void near select_draw_page(void)
{
    if (r_vid.mode == VID_EGA) {
        g_videoAddrSegment = (g_viewPages.draw == 1) ? EGA_PAGE1_SEGMENT : EGA_PAGE2_SEGMENT;
        ep_show(g_viewPages.show);
    } else {
        g_videoAddrSegment = g_driveScreenB->bits.w.seg;
        g_targetPixelOffset = g_driveScreenB->bits.w.off;
    }
}

struct Bitmap far *g_lightsBm;
struct Bitmap far *g_windowStrutArt;
struct Bitmap far *g_citySkylineBmp;
struct Bitmap far *g_skylineCountry;
struct Bitmap far *g_imageFullMirror;
struct Bitmap far *g_rearBasePic;
int far *g_viewTerrainBitsData;
struct Bitmap far *g_windowBorderImage;


static void near flip_page(void)
{
    int t;

    if (r_vid.mode == VID_EGA) {
        t = g_viewPages.draw;
        g_viewPages.draw = g_viewPages.show;
        g_viewPages.show = t;
        g_videoAddrSegment = (g_viewPages.draw == 1) ? EGA_PAGE1_SEGMENT : EGA_PAGE2_SEGMENT;
        ep_show(g_viewPages.show);
    } else {
        (*g_videoOps.copy)(g_driveScreenB, r_vid.front, (struct Blt *)&g_viewRect, 1);
        g_videoAddrSegment = g_driveScreenB->bits.w.seg;
        g_targetPixelOffset = g_driveScreenB->bits.w.off;
    }
}

/* Clips the vertical extent of a blit rectangle to the clip window. */

static void near clip_blit_y(struct Blt *b)
{
    if (b->dy < g_viewClip.yMin) {
        b->h -= g_viewClip.yMin - b->dy;
        b->sy += g_viewClip.yMin - b->dy;
        b->dy = g_viewClip.yMin;
    }
    if (g_viewClip.yMax < b->dy + b->h)
        b->h -= b->dy + b->h - g_viewClip.yMax;
}

/* Clips a blit rectangle to the clip window, vertically and then
   horizontally. */

static void near clip_blit(struct Blt *b)
{
    if (b->dy < g_viewClip.yMin) {
        b->h -= g_viewClip.yMin - b->dy;
        b->sy += g_viewClip.yMin - b->dy;
        b->dy = g_viewClip.yMin;
    }
    if (g_viewClip.yMax < b->dy + b->h)
        b->h -= b->dy + b->h - g_viewClip.yMax;
    if (b->dx < g_viewClip.xMin) {
        b->w -= g_viewClip.xMin - b->dx;
        b->sx += g_viewClip.xMin - b->dx + 7;
        b->dx = g_viewClip.xMin;
    }
}

/* Draws a clipped vertical line (color, x, y1, y2). */

static void near clip_vline(int color, int x, int y1, int y2)
{
    if (y2 >= g_viewClip.yMin && y1 <= g_viewClip.yMax && x >= g_viewClip.xMin && x <= g_viewClip.xMax) {
        y1 = (y1 < g_viewClip.yMin) ? g_viewClip.yMin : y1;
        y1 = (y1 > g_viewClip.yMax) ? g_viewClip.yMax : y1;
        y2 = (y2 < g_viewClip.yMin) ? g_viewClip.yMin : y2;
        y2 = (y2 > g_viewClip.yMax) ? g_viewClip.yMax : y2;
        (*g_videoOps.drawLine)(x, y1, x, y2, color);
    }
}


void vw_unclip_top(void)
{
    g_viewClip.yMin = 0;
}

/* Sets the clip window to x 242..317, y 22..42. */

static void near clip_mirror(void)
{
    g_viewClip.xMin = 0xf2;
    g_viewClip.yMin = 0x16;
    g_viewClip.xMax = 0x13d;
    g_viewClip.yMax = 0x2a;
}

/* Sets the clip window to x 0..319, y 18..101. */

void vw_clip(void)
{
    g_viewClip.xMin = 0;
    g_viewClip.yMin = 0x12;
    g_viewClip.yMax = 0x65;
    g_viewClip.xMax = (SCREEN_WIDTH - 1);
}


void vw_load(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *(far * load)(struct Bitmap far *, unsigned char, unsigned char);
    struct Blt pkt1;
    int sel;
    struct Bitmap far *bm24;
    int il;
    int idx;

    ma_clear();
    bm1 = di_bm(PIC_DASHBOARD_TOP, ARENA_HIGH_STACK);
    pkt1.w = bm1->width;
    pkt1.h = bm1->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_videoOps.copy)(bm1, g_driveScreenB, &pkt1, 1);
    ma_pop_high(1);
    sel = *(int *)&g_runRacingFlag == 0 ? g_scene : 0;
    bm24 = di_bm(g_sceneBitmapIds[sel], ARENA_HIGH_STACK);
    pkt1.w = bm24->width;
    pkt1.h = bm24->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_videoOps.copy)(bm24, g_bufferSceneA, &pkt1, 1);
    ma_pop_high(1);
    if (*(int *)&g_runRacingFlag == 0) {
        bm24 = di_bm(PIC_SIGN_VEGAS_GAMBLER, ARENA_HIGH_STACK);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 1;
        (*g_videoOps.copy)(bm24, r_vid.mode == VID_EGA ? g_driveDisplayPage : g_bufferSceneA, &pkt1, 1);
        ma_pop_high(1);
        bm24 = di_bm(PIC_SIGN_BLOCK_OUT, ARENA_HIGH_STACK);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 0x2c;
        (*g_videoOps.copy)(bm24, r_vid.mode == VID_EGA ? g_driveDisplayPage : g_bufferSceneA, &pkt1, 1);
        ma_pop_high(1);
    }
    g_skylineCountry = di_bm(PIC_COUNTRY_SKYLINE, ARENA_LOW_STACK);
    if (*(int *)&g_runRacingFlag == 0) {
        g_citySkylineBmp = di_bm(PIC_CITY_SKYLINE, ARENA_LOW_STACK);
        pkt1.w = g_citySkylineBmp->width;
        pkt1.h = g_citySkylineBmp->height;
        pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x2c;
        pkt1.dy = 0xc;
        (*g_videoOps.maskedBlit)(g_citySkylineBmp, g_skylineCountry, &pkt1, 1);
        ma_pop_low(1);
    }
    pkt1.w = g_skylineCountry->width;
    pkt1.h = g_skylineCountry->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = SCREEN_HEIGHT - g_skylineCountry->height;
    (*g_videoOps.copy)(g_skylineCountry, g_driveScreenB, &pkt1, 1);
    ma_pop_low(1);
    g_windowStrutArt = di_bm(PIC_WINDSHIELD_STRUT, ARENA_LOW_STACK);
    g_windowBorderImage = di_bm(PIC_HOOD_EDGE, ARENA_LOW_STACK);
    g_imageFullMirror = di_bm(PIC_REARVIEW_MIRROR, ARENA_LOW_STACK);
    g_rearBasePic = di_bm(PIC_MIRROR_LANDSCAPE, ARENA_LOW_STACK);
    if (*(int *)&g_runRacingFlag) {
        idx = g_models[g_opponentSpecIndex].kind;
        g_lightsBm = di_bm(PIC_START_LIGHTS, ARENA_LOW_STACK);
        g_startDrawRows = g_lightsBm->height;
        g_spritesOpp[0] = di_bm(g_opponentPics[idx][0], ARENA_LOW_STACK);
        g_spritesOpp[1] = di_bm(g_opponentPics[idx][1], ARENA_LOW_STACK);
        g_spritesOpp[2] = di_bm(g_opponentPics[idx][2], ARENA_HIGH_STACK);
        load = r_vid.mode == VID_EGA ? sr_ega : r_vid.mode == VID_TANDY ? sr_tandy : sr_cga;
        for (il = 0; il < 3; il++)
            g_spritesOpp[3 + il] = (*load)(g_spritesOpp[2], g_shrinkMasks[0][il], g_shrinkMasks[0][il]);
        for (il = 0; il < 3; il++)
            g_spritesOpp[6 + il] = (*load)(g_spritesOpp[5], g_shrinkMasks[1][il], g_shrinkMasks[1][il]);
        g_spritesOpp[9] = di_bm(g_opponentPics[idx][3], ARENA_LOW_STACK);
        for (il = 0; il < 3; il++)
            g_spritesOpp[10 + il] = (*load)(g_spritesOpp[9], g_shrinkMasks[2][il], g_shrinkMasks[2][il]);
        g_policeFramesTable[0] = g_spritesOpp[0];
        g_policeFramesTable[1] = di_bm(PIC_POLICE_REAR_ANGLED, ARENA_LOW_STACK);
        g_policeFramesTable[2] = di_bm(PIC_POLICE_REAR, ARENA_HIGH_STACK);
        g_policeFramesTable[2] = g_policeFramesTable[3] =
            (*load)(g_policeFramesTable[2], g_shrinkMasks[0][0], g_shrinkMasks[0][0]);
        ma_pop_high(1);
        for (il = 0; il < 3; il++)
            g_policeFramesTable[4 + il] = g_policeFramesTable[3];
        g_policeFramesTable[9] = di_bm(PIC_POLICE_DISTANT_REAR, ARENA_LOW_STACK);
        for (il = 0; il < 3; il++)
            g_policeFramesTable[10 + il] = (*load)(g_policeFramesTable[9], g_shrinkMasks[2][il], g_shrinkMasks[2][il]);
    }
}


static void near render_backdrop(void)
{
    int r;
    int n8 = 0xf0;
    int m5 = 1000;
    struct Blt b;
    int v;
    int ll;
    int i;

    b.dy = 0x12;
    if (*(int *)&g_runRacingFlag) {
        v = rd_proj_y(n8, m5 * 2, g_trackViewMotion.roadHeight);
        b.h = v - 0x1e;
        b.sy = 0x9a - b.h;
        if (g_trackViewMotion.curveX > 0) {
            r = (g_trackViewMotion.curveX * 2 % SCREEN_WIDTH) & ~7;
            ll = SCREEN_WIDTH - r;
        } else {
            ll = (abs(g_trackViewMotion.curveX) * 2 % SCREEN_WIDTH) & ~7;
            r = SCREEN_WIDTH - ll;
        }
        b.w = ll;
        b.sx = 0;
        b.dx = r;
    } else {
        b.w = SCREEN_WIDTH;
        b.sx = 0;
        b.sy = 0x66;
        b.dx = 0;
        b.h = 0x1d;
    }
    (*g_videoOps.copy)(g_driveScreenB, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    if (*(int *)&g_runRacingFlag) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_videoOps.copy)(g_driveScreenB, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    }
    b.h = *(int *)&g_runRacingFlag ? 0x14 : 0x10;
    b.sy = 0xa3;
    if (*(int *)&g_runRacingFlag) {
        if (g_trackViewMotion.curveX > 0) {
            r = (g_trackViewMotion.curveX * 4 % SCREEN_WIDTH) & ~7;
            ll = SCREEN_WIDTH - r;
        } else {
            ll = (abs(g_trackViewMotion.curveX) * 4 % SCREEN_WIDTH) & ~7;
            r = SCREEN_WIDTH - ll;
        }
        b.w = ll;
        b.sx = 0;
        b.dx = r;
        b.dy = v - 0xc;
    } else {
        b.w = SCREEN_WIDTH;
        b.sx = 0;
        b.dx = 0;
        b.dy = 0x2f;
    }
    clip_blit_y(&b);
    (*g_videoOps.copy)(g_driveScreenB, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    g_roadRowLimit = 14;
    if (*(int *)&g_runRacingFlag) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_videoOps.copy)(g_driveScreenB, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
        g_horizonY = v + 8;
        for (i = 14; i; i--) {
            if (g_roadYProjectionArray[i] < g_horizonY) {
                g_horizonY = g_roadYProjectionArray[i];
                g_roadRowLimit = i;
            }
        }
        (*g_videoOps.fillBelow)(g_horizonY, 9);
    } else
        (*g_videoOps.fillBelow)(0x3f, 1);
}


void vw_sprite(int x, int y, int first, struct List l, int dir)
{
    struct Run run;
    int px;
    int x2;
    int y1;
    int j;

    if (l.last > first) {
        run = l.items[first];
        if (dir == -1) {
            for (j = 0; j < run.n; j++) {
                px = x - run.parts[j].dx;
                x2 = px - run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_videoOps.fillSpan)(x2, px, y1, run.parts[j].color);
            }
        } else {
            for (j = 0; j < run.n; j++) {
                px = run.parts[j].dx + x;
                x2 = px + run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_videoOps.fillSpan)(px, x2, y1, run.parts[j].color);
            }
        }
    }
}


static int near edge_step(int x1, int y1, int x2, int y2)
{
    if (y2 == y1)
        return 0;
    return ((x2 - x1) << 4) / (y2 - y1);
}


static void near fill_quad(int col, int a, int b, int c, int d, int e, int f)
{
    enum QuadShape mode;
    int dxa;
    int dxb;
    int racc6;
    int laccl;
    int xr5;
    int xlk;
    int ymin2;
    int y6;
    int ymax;

    if (c == d && f == e) {
        for (y6 = f; y6 <= d; y6++)
            (*g_videoOps.fillSpan)(a, b, y6, col);
    } else {
        ymin2 = f > e ? e : f;
        ymax = c < d ? d : c;
        if (f < e && c > d) {
            mode = QUAD_LEFT_ENCLOSES_RIGHT;
            xlk = a;
            racc6 = a << 4;
        } else if (e < f && d > c) {
            mode = QUAD_RIGHT_ENCLOSES_LEFT;
            xr5 = b;
            xlk = b << 4;
        } else {
            if (f <= e && c <= d) {
                mode = QUAD_LEFT_HIGHER;
                racc6 = a << 4;
            } else {
                mode = QUAD_RIGHT_HIGHER;
                racc6 = b << 4;
            }
            laccl = racc6;
        }
        dxa = edge_step(a, f, b, e);
        dxb = edge_step(a, c, b, d);
        y6 = ymin2;
        goto test;
    loop:
        switch (mode) {
        case QUAD_LEFT_ENCLOSES_RIGHT:
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else if (y6 <= d)
                xr5 = b;
            else
                xr5 = (racc6 += dxb) >> 4;
            break;
        case QUAD_RIGHT_ENCLOSES_LEFT:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            break;
        case QUAD_LEFT_HIGHER:
            if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else
                xr5 = b;
            break;
        case QUAD_RIGHT_HIGHER:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else
                xlk = a;
            if (y6 > d)
                xr5 = (racc6 += dxb) >> 4;
            else
                xr5 = b;
            break;
        }
        if (xlk > xr5)
            (*g_videoOps.fillSpan)(xr5, xlk, y6, col);
        else
            (*g_videoOps.fillSpan)(xlk, xr5, y6, col);
        y6++;
    test:
        if (y6 <= ymax && y6 <= g_viewClip.yMax)
            goto loop;
    }
}


static int near sgn(int n)
{
    if (n > 0)
        return 1;
    if (n < 0)
        return -1;
    return 0;
}


void vw_road_row(int n)
{
    int xl;
    int ybot6;
    int xr28;
    int yk;
    int wid;
    int cnt;
    int m17;
    int dxr7;
    int t1;
    int xlsn;
    int xr2s6;
    int dxl2k;
    int dxlm;
    int flagk;
    int dxr26;
    int t2;
    int xl2;
    int last2;
    int xrl;
    int cx0k;
    int xrs;
    int xl2s6;
    int m2;

    xl = *g_renderRows.leftX;
    xrl = *g_renderRows.rightX;
    xlsn = xl << 4;
    xrs = xrl << 4;
    yk = *g_renderRows.y;
    ybot6 = *g_renderRows.prevY;
    dxlm = edge_step(*g_renderRows.prevLeftX, ybot6, xl, yk);
    dxr7 = edge_step(*g_renderRows.prevRightX, ybot6, xrl, yk);
    flagk = *g_viewTerrainBitsData & 1;
    if (flagk) {
        xl2 = *g_renderRows.leftOuterX;
        xr28 = *g_renderRows.rightOuterX;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = edge_step(*g_renderRows.prevLeftOuterX, ybot6, xl2, yk);
        dxr26 = edge_step(*g_renderRows.prevRightOuterX, ybot6, xr28, yk);
    }
    if (*g_viewTerrainBitsData & 8) {
        wid = *g_renderRows.lineWidth;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (sgn(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 101; cnt--, yk++) {
        if (flagk) {
            if (*g_viewTerrainBitsData & 4)
                break;
            if (*g_renderRows.leftFlags & 0x80) {
                m17 = xl2 + ((xl - xl2) >> 1);
                (*g_videoOps.fillSpan)(xl2, m17, yk, 4);
                (*g_videoOps.fillSpan)(m17, xl, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_videoOps.fillSpan)(xl2, m17, yk, 2);
            } else {
                (*g_videoOps.fillSpan)(xl2, xl, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_videoOps.fillSpan)(xl2, xl, yk, 2);
            }
            if (*g_renderRows.rightFlags & 0x80) {
                m2 = xr28 - ((xr28 - xrl) >> 1);
                (*g_videoOps.fillSpan)(m2, xr28, yk, 4);
                (*g_videoOps.fillSpan)(xrl, m2, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_videoOps.fillSpan)(m2, xr28, yk, 2);
            } else {
                (*g_videoOps.fillSpan)(xrl, xr28, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_videoOps.fillSpan)(xrl, xr28, yk, 2);
            }
        } else {
            (*g_videoOps.fillSpan)(xl, xrl, yk, (*g_viewTerrainBitsData & 0x10) ? 15 : 1);
        }
        if (*g_viewTerrainBitsData & 8) {
            t1 = cx0k;
            t2 = t1 + wid;
            (*g_videoOps.fillSpan)(t1, t2, yk, 4);
        }
        xlsn += dxlm;
        xrs += dxr7;
        xl = xlsn >> 4;
        xrl = xrs >> 4;
        if (flagk) {
            xl2s6 += dxl2k;
            xr2s6 += dxr26;
            xl2 = xl2s6 >> 4;
            xr28 = xr2s6 >> 4;
        }
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
}


void vw_mirror_row(void)
{
    int xl;
    int ybot6;
    int xr28;
    int yk;
    int wid;
    int cnt;
    int m17;
    int dxr7;
    int t1;
    int xlsn;
    int xr2s6;
    int dxl2k;
    int dxlm;
    int flagk;
    int dxr26;
    int t2;
    int xl2;
    int xrl;
    int cx0k;
    int xrs;
    int xl2s6;
    int m2;

    xl = *g_renderRows.leftX;
    xrl = *g_renderRows.rightX;
    xlsn = xl << 4;
    xrs = xrl << 4;
    yk = *g_renderRows.y;
    ybot6 = *g_renderRows.prevY;
    dxlm = edge_step(*g_renderRows.prevLeftX, ybot6, xl, yk);
    dxr7 = edge_step(*g_renderRows.prevRightX, ybot6, xrl, yk);
    flagk = *g_viewTerrainBitsData & 1;
    if (flagk) {
        xl2 = *g_renderRows.leftOuterX;
        xr28 = *g_renderRows.rightOuterX;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = edge_step(*g_renderRows.prevLeftOuterX, ybot6, xl2, yk);
        dxr26 = edge_step(*g_renderRows.prevRightOuterX, ybot6, xr28, yk);
    }
    if (*g_viewTerrainBitsData & 8) {
        wid = *g_renderRows.lineWidth;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (sgn(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 42; cnt--, yk++) {
        if (flagk) {
            if (*g_viewTerrainBitsData & 4)
                break;
            if (*g_renderRows.leftFlags & 0x80) {
                m17 = xl2 + ((xl - xl2) >> 1);
                (*g_videoOps.fillSpan)(xl2, m17, yk, 4);
                (*g_videoOps.fillSpan)(m17, xl, yk, 9);
            } else {
                (*g_videoOps.fillSpan)(xl2, xl, yk, 4);
            }
            if (*g_renderRows.rightFlags & 0x80) {
                m2 = xr28 - ((xr28 - xrl) >> 1);
                (*g_videoOps.fillSpan)(m2, xr28, yk, 4);
                (*g_videoOps.fillSpan)(xrl, m2, yk, 9);
            } else {
                (*g_videoOps.fillSpan)(xrl, xr28, yk, 4);
            }
        } else {
            (*g_videoOps.fillSpan)(242, xl, yk, 9);
            (*g_videoOps.fillSpan)(xrl, 317, yk, 9);
            (*g_videoOps.fillSpan)(xl, xrl, yk, 1);
        }
        if (*g_viewTerrainBitsData & 8) {
            t1 = cx0k;
            t2 = t1 + wid;
            (*g_videoOps.fillSpan)(t1, t2, yk, 4);
        }
        xlsn += dxlm;
        xrs += dxr7;
        xl = xlsn >> 4;
        xrl = xrs >> 4;
        if (flagk) {
            xl2s6 += dxl2k;
            xr2s6 += dxr26;
            xl2 = xl2s6 >> 4;
            xr28 = xr2s6 >> 4;
        }
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
}


static void near render_buildings(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b8l;
    int b4;
    int t8l;
    int y;

    t8l = *g_viewTerrainBitsData & 1;
    if (rev) {
        a41 = g_renderRows.leftFlags[0] & 4;
        b8l = g_renderRows.leftFlags[0] & 8;
        b4 = g_renderRows.leftFlags[-1] & 4;
        c10 = g_renderRows.leftFlags[0] & 0x10;
    } else {
        a41 = g_renderRows.rightFlags[0] & 4;
        b8l = g_renderRows.rightFlags[0] & 8;
        b4 = g_renderRows.rightFlags[-1] & 4;
        c10 = g_renderRows.rightFlags[0] & 0x10;
    }
    if (!(*g_viewTerrainBitsData & 4)) {
        if (c10)
            fill_quad(2, a, b, c, d, e, f);
        else
            fill_quad(3, a, b, c, d, e, f);
        if (a41) {
            if (b8l) {
                fill_quad(4, a, b, f, e, e - d + e, f - c + f);
            } else {
                fill_quad(4, a, b, f, e, e - ((d - e) >> 1), f - ((c - f) >> 1));
            }
        }
        if (g_viewTerrainBitsData[-1] & 4) {
            if (rev)
                (*g_videoOps.fillSpan)(g_viewClip.xMin, a, c - 1, 4);
            else
                (*g_videoOps.fillSpan)(b, g_viewClip.xMax, d - 1, 4);
            if (rev) {
                for (y = f; y < c - 1; y++)
                    (*g_videoOps.fillSpan)(g_viewClip.xMin, a, y, 2);
            } else {
                for (y = e; y < d - 1; y++)
                    (*g_videoOps.fillSpan)(b, g_viewClip.xMax, y, 2);
            }
        }
        if (a41 && !b4) {
            int x1;
            int x2;

            if (b8l) {
                if (rev) {
                    y = f - c + f;
                    if (y < f) {
                        x1 = a - b + a;
                        x2 = a - 1;
                        do
                            (*g_videoOps.fillSpan)(x1, x2, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - d + e;
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_videoOps.fillSpan)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            } else {
                if (rev) {
                    y = f - ((c - f) >> 1);
                    if (y < f) {
                        x2 = a - b + a;
                        x1 = a - 1;
                        do
                            (*g_videoOps.fillSpan)(x2, x1, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - ((d - e) >> 1);
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_videoOps.fillSpan)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            }
        }
    }
}


static void near render_mirror_buildings(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b4;
    int t8l;
    int y;

    if (t8l = *g_viewTerrainBitsData & 1) {
        if (rev) {
            a41 = g_renderRows.leftFlags[0] & 4;
            b4 = g_renderRows.leftFlags[1] & 4;
            c10 = g_renderRows.leftFlags[0] & 0x10;
        } else {
            a41 = g_renderRows.rightFlags[0] & 4;
            b4 = g_renderRows.rightFlags[1] & 4;
            c10 = g_renderRows.rightFlags[0] & 0x10;
        }
        if (!(*g_viewTerrainBitsData & 4)) {
            if (c10)
                fill_quad(2, a, b, c, d, e, f);
            else
                fill_quad(3, a, b, c, d, e, f);
            if (a41) {
                if (rev)
                    fill_quad(4, a, b, f, e, e - ((d - e) >> 1), f - ((c - f) >> 1));
                else
                    fill_quad(4, a, b, f, e, e - d + e, f - c + f);
            }
            if (g_viewTerrainBitsData[1] & 4) {
                if (rev)
                    (*g_videoOps.fillSpan)(g_viewClip.xMin, a, c - 1, 4);
                else
                    (*g_videoOps.fillSpan)(b, g_viewClip.xMax, d - 1, 4);
                if (rev) {
                    for (y = f; y < c - 1; y++)
                        (*g_videoOps.fillSpan)(g_viewClip.xMin, a, y, 2);
                } else {
                    for (y = e; y < d - 1; y++)
                        (*g_videoOps.fillSpan)(b, g_viewClip.xMax, y, 2);
                }
            }
            if (a41 && !b4) {
                int x1;
                int x2;

                if (rev) {
                    y = f - ((c - f) >> 1);
                    if (y < f) {
                        x1 = a - b + a;
                        x2 = a - 1;
                        do
                            (*g_videoOps.fillSpan)(x1, x2, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - d + e;
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_videoOps.fillSpan)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            }
        }
    }
}


static void near render_left_buildings(void)
{
    render_buildings(*g_renderRows.prevLeftOuterX, *g_renderRows.leftOuterX, *g_renderRows.prevY, *g_renderRows.y,
                     *g_renderRows.buildingY, *g_renderRows.prevBuildingY, 1);
}


static void near render_right_buildings(void)
{
    render_buildings(*g_renderRows.rightOuterX, *g_renderRows.prevRightOuterX, *g_renderRows.y, *g_renderRows.prevY,
                     *g_renderRows.prevBuildingY, *g_renderRows.buildingY, 0);
}


static void near render_row(int n)
{
    if (*(int *)&g_runRacingFlag == 0) {
        render_left_buildings();
        render_right_buildings();
    }
    vw_road_row(n);
}


static void near render_mirror_left(void)
{
    render_mirror_buildings(*g_renderRows.prevLeftOuterX, *g_renderRows.leftOuterX, *g_renderRows.prevY,
                            *g_renderRows.y, *g_renderRows.buildingY, *g_renderRows.prevBuildingY, 1);
}


static void near render_mirror_right(void)
{
    render_mirror_buildings(*g_renderRows.rightOuterX, *g_renderRows.prevRightOuterX, *g_renderRows.y,
                            *g_renderRows.prevY, *g_renderRows.prevBuildingY, *g_renderRows.buildingY, 0);
}


static void near render_mirror_row(void)
{
    render_mirror_left();
    render_mirror_right();
    vw_mirror_row();
}


void vw_draw_dot(int x, int y)
{
    en_begin();
    vw_sprite(x, y, 0, *g_scenery[SCENERY_DOT], 1);
    en_end();
}


static void near render_object(int x, int y, int n, int f, enum SceneryId idx)
{
    n = 9 - n;
    if (n >= 7)
        n = 6;
    if (n < 0)
        n = 0;
    if (y > 0x65)
        y = 0x65;
    vw_sprite(x, y, n, *g_scenery[idx], f);
}


static void near render_hydrant(int x, int y, int n, int f)
{
    n = 9 - n;
    if (n >= 8)
        n = 7;
    if (n < 0)
        n = 0;
    vw_sprite(x, y, n, *g_scenery[SCENERY_HYDRANT], f);
}


static void near render_scaled_pic(int x, int y, int n, int align, int clip, int mode)
{
    struct Blt b;

    n = 9 - n;
    if (n > 6)
        n = 6;
    if (n < 0)
        n = 0;
    b.w = g_sheetRects[n].w;
    b.h = g_sheetRects[n].h;
    b.sx = g_sheetRects[n].sx;
    switch (mode) {
    case 1:
        b.sy = 0x66;
        break;
    case 2:
        b.sy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 1;
        break;
    case 3:
        b.sy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 0x2c;
        break;
    }
    if (align == 1)
        b.dx = x - b.w;
    else if (align == 0)
        b.dx = x - (b.w >> 1);
    else
        b.dx = x;
    if (align == 0)
        b.dy = y - (b.h >> 1);
    else
        b.dy = y - b.h;
    if (clip)
        clip_blit(&b);
    else
        clip_blit_y(&b);
    if (mode == 1)
        (*g_videoOps.copy)(g_bufferSceneA, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    else
        (*g_videoOps.copy)(r_vid.mode == VID_EGA ? g_driveDisplayPage : g_bufferSceneA,
                           g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
}


static void near render_gantry(int x1, int x2, int y1, int y2, int n, int clip, int flags)
{
    int j;

    for (j = *g_renderRows.lineWidth + 1; j; j--) {
        en_end();
        clip_vline(10, x1 - j, y1, y2);
        clip_vline(10, x2 + j, y1, y2);
        en_begin();
        (*g_videoOps.fillSpan)(x1, x2, y1 + j - 1, 10);
    }
    if (flags & 0x1000)
        render_object((x1 + x2) >> 1, y1, n, 1, SCENERY_CITY_LIMITS);
    else if (flags & 0x2000) {
        en_end();
        render_scaled_pic((x1 + x2) >> 1, y1, n, 0, clip, 1);
        en_begin();
    } else if (*g_viewTerrainBitsData & 1) {
        en_end();
        render_scaled_pic((x1 + x2) >> 1, y1, n, 0, clip, (flags & 0x4000) ? 2 : 3);
        en_begin();
    } else
        render_object((x1 + x2) >> 1, y1, n, 1, (flags & 0x4000) ? 12 : 14);
}


static void near render_start_signal(void)
{
    struct Blt b;

    if (g_startDrawRows > 0) {
        b.w = 0x18;
        b.h = g_startDrawRows;
        if (g_startingGoActive) {
            g_startLightGo = 1;
            b.sx = 0;
        } else
            b.sx = 0x18;
        b.sy = 0x20 - g_startDrawRows;
        b.dx = 0x82;
        b.dy = 0x12;
        (*g_videoOps.maskedBlit)(g_lightsBm, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    }
}


static void near render_overlays(void)
{
    struct Blt b;

    b.w = g_windowStrutArt->width;
    b.h = g_windowStrutArt->height;
    b.dx = b.sx = b.sy = 0;
    b.dy = 0x12;
    (*g_videoOps.maskedBlit)(g_windowStrutArt, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    b.w = g_windowBorderImage->width;
    b.h = g_windowBorderImage->height;
    b.sx = b.sy = 0;
    b.dx = 0x58;
    b.dy = 0x5f;
    (*g_videoOps.maskedBlit)(g_windowBorderImage, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
}


static void near render_mirror_frame(void)
{
    struct Blt b;

    b.w = g_imageFullMirror->width;
    b.h = g_imageFullMirror->height;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = 0x12;
    (*g_videoOps.copy)(g_imageFullMirror, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
    b.w = g_rearBasePic->width;
    b.h = 2;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = g_mirYProj[7] - 2;
    clip_blit_y(&b);
    (*g_videoOps.copy)(g_rearBasePic, g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
}


static void near render_opponent(void)
{
    struct Blt b;
    int seln;
    int ra;
    int flag9;
    int z9;
    int a2;
    int xk;
    int rb;
    int t1;
    int rc;
    int rd;
    struct Bitmap far *bm;

    a2 = g_roadOppPos.roadIndex;
    xk = (a2 - g_trackViewMotion.roadIndex) * 8 + g_roadOppPos.roadFraction - g_trackViewMotion.roadFraction;
    z9 = g_roadOppPos.lateralX - g_trackViewMotion.lateralX;
    if (xk >= 0x78)
        seln = -1;
    else if (xk >= 0x10) {
        if ((seln = ((xk + 8) >> 4) + 2) > 8)
            seln = 8;
    } else if (xk < -7)
        seln = ((0x10 - xk) >> 4) + 8;
    else if (xk <= 8) {
        if (abs(z9) < 180) {
            if (g_picOpponent <= 8)
                seln = 2;
            else
                seln = 9;
        } else
            seln = z9 < 0 ? 0 : 2;
    } else if (z9 < -180)
        seln = 1;
    else
        seln = 3;
    if (g_processorSpeedRating >= 4 && seln != g_picOpponent) {
        if (seln < 0) {
            if (g_picOpponent == 8)
                return;
            seln = g_picOpponent + 1;
        } else if (seln > 12) {
            if (g_picOpponent == 12)
                return;
            seln = g_picOpponent + 1;
        } else if (seln != g_prevRivalFrame) {
            if ((seln <= 8 && g_picOpponent > 9) || (seln >= 9 && g_picOpponent <= 8 && g_picOpponent > 0))
                seln = g_picOpponent - 1;
            else if (g_picOpponent == 0)
                seln = seln < 9 ? 1 : 9;
            else if (g_picOpponent == 9)
                seln = seln < 9 ? (z9 < 0 ? 0 : 2) : 10;
            else if (g_picOpponent == 1)
                seln = seln < 1 ? 0 : 3;
            else if (g_picOpponent == 3 && seln == 1)
                ;
            else if (seln > g_picOpponent)
                seln = g_picOpponent + 1;
            else
                seln = g_picOpponent - 1;
        }
    }
    g_carCollisionRange = seln == 0 || (seln == 2 && xk <= 10);
    g_carHit = seln == 2 && xk <= 10 && abs(z9) < 180 && (g_processorSpeedRating < 4 ? 100 : 180) > abs(z9) &&
               g_roadOppPos.distanceStep < g_trackViewMotion.distanceStep;
    g_prevRivalFrame = g_picOpponent;
    g_picOpponent = seln;
    g_unused22 = xk > 16 ? (xk + 8) & 15 : 0;
    t1 = xk;
    if (seln == 0)
        t1 += 16;
    if (t1 < -2 && seln == 2)
        t1 += 8;
    rc = g_trackViewMotion.roadHeight - g_terrain4[a2];
    rd = g_trackViewMotion.curveX - g_terrain2[a2];
    if (seln < 9) {
        t1 += 4;
        ra = rd_proj_x(t1, g_roadOppPos.lateralX, rd);
        rb = rd_proj_y(t1, ROAD_SURFACE_Y, rc);
    } else {
        t1 = -t1;
        ra = rd_mirror_x(t1, g_roadOppPos.lateralX, -rd);
        rb = rd_mirror_y(t1, ROAD_SURFACE_Y, -rc);
    }
    if (seln == 0) {
        bm = g_spritesOpp[seln];
        b.w = bm->width;
        b.h = bm->height;
        if ((b.sx = b.w - ra + 0x40) < 0)
            b.sx = 0;
        b.dx = 0;
        b.sy = 0;
        b.dy = rb;
    } else if (seln == 1) {
        bm = g_spritesOpp[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - b.w;
        b.sy = 0;
        b.dy = rb - b.h + 0x11;
    } else {
        bm = g_spritesOpp[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - (b.w >> 1);
        b.sy = 0;
        b.dy = (seln < 9 ? (rb < g_horizonY ? g_horizonY : rb) : rb) - b.h;
        if (seln == 2 && b.dy + b.h >= 0x65)
            b.dy = 0x65 - b.h;
    }
    flag9 = 0;
    if (seln >= 9) {
        if (g_viewClip.xMin == 0) {
            flag9 = 1;
            clip_mirror();
        }
    } else if (g_viewClip.xMin != 0) {
        flag9 = 1;
        vw_clip();
    }
    if (b.dx + b.w >= g_viewClip.xMin && b.dx < g_viewClip.xMax) {
        if (seln >= 9)
            clip_blit(&b);
        else
            clip_blit_y(&b);
        (*g_videoOps.maskedBlit)(g_spritesOpp[seln], g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, &b, 1);
        if (flag9) {
            if (g_viewClip.xMin == 0)
                clip_mirror();
            else
                vw_clip();
        }
    }
}


static void near render_mirror(void)
{
    int w36;
    struct Blt pkt1;
    enum RoadsideFlags w11;
    int i;
    int f3;
    enum RoadsideFlags w2;

    g_renderRows.leftX = &g_lhsRdMirrorX[7];
    g_renderRows.rightX = &g_mirGeom.rightX[7];
    g_renderRows.y = &g_mirYProj[7];
    g_renderRows.leftOuterX = &g_edgesRoad.leftOuterX[7];
    g_renderRows.rightOuterX = &g_edgesRoad.rightOuterX[7];
    g_renderRows.buildingY = &g_mirGeom.buildingY[7];
    g_renderRows.prevLeftX = &g_lhsRdMirrorX[6];
    g_renderRows.prevRightX = &g_mirGeom.rightX[6];
    g_renderRows.prevY = &g_mirYProj[6];
    g_renderRows.prevBuildingY = &g_mirGeom.buildingY[6];
    g_renderRows.prevLeftOuterX = &g_edgesRoad.leftOuterX[6];
    g_renderRows.prevRightOuterX = &g_edgesRoad.rightOuterX[6];
    g_renderRows.lineWidth = &g_roadLineWidths[20];
    g_renderRows.leftFlags = (int far *)(g_roadTerrain - 8);
    g_renderRows.rightFlags = (int far *)(g_rightSideTerrainRow - 8);
    g_viewTerrainBitsData = (int far *)(g_hereFlagsPtr - 8);
    en_end();
    render_mirror_frame();
    en_begin();
    clip_mirror();
    for (i = 7; i; i--) {
        render_mirror_row();
        if (*g_viewTerrainBitsData & 4)
            goto next;
        if (*g_viewTerrainBitsData & 0xf000)
            render_gantry(*g_renderRows.leftX, *g_renderRows.rightX,
                          -((*g_renderRows.y - *g_renderRows.buildingY) * 4 - *g_renderRows.y), *g_renderRows.y, i + 5,
                          1, *g_viewTerrainBitsData & 0xf000);
        if (i >= 6)
            goto next;
        f3 = *g_viewTerrainBitsData & 1;
        w11 = *g_renderRows.leftFlags;
        w2 = *g_renderRows.rightFlags;
        w36 = *g_viewTerrainBitsData;
        if (w11 & ROADSIDE_TREE) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, SCENERY_TREE);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, SCENERY_TREE);
        }
        if (w11 & ROADSIDE_BUSH) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, SCENERY_BUSH);
            else if (w11 & ROADSIDE_BUSH_OUTER)
                render_object(*g_renderRows.leftOuterX - *g_renderRows.leftX + *g_renderRows.leftOuterX,
                              *g_renderRows.y, i + 5, -1, SCENERY_BUSH);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, SCENERY_BUSH);
        }
        if (w11 & ROADSIDE_WARNING)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, SCENERY_WARNING_SIGN);
        if (w11 & ROADSIDE_BARRICADE)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, SCENERY_BARRICADE);
        if (f3) {
            if (w11 & 0x40)
                render_hydrant(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1);
            if (w2 & 0x40)
                render_hydrant(*g_renderRows.rightX, *g_renderRows.y, i + 5, -1);
        } else {
            if (w11 & ROADSIDE_POST)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, SCENERY_POST);
            if (w11 & ROADSIDE_SMALL_ROCK)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, SCENERY_SMALL_ROCK);
            if (w11 & ROADSIDE_LARGE_ROCK)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, SCENERY_LARGE_ROCK);
            if (w11 & 0x40)
                render_object(-(((*g_renderRows.leftX - *g_renderRows.leftOuterX) >> 1) - *g_renderRows.leftOuterX),
                              *g_renderRows.y, i + 5, -1, SCENERY_TREE_2);
            if (w11 & ROADSIDE_TREE_3)
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, SCENERY_TREE_3);
            if (w2 & ROADSIDE_POST)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_POST);
            if (w2 & ROADSIDE_SMALL_ROCK)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_SMALL_ROCK);
            if (w2 & ROADSIDE_LARGE_ROCK)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_LARGE_ROCK);
            if (w2 & 0x40)
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, SCENERY_TREE_2);
            if (w2 & ROADSIDE_TREE_3)
                render_object(((*g_renderRows.rightOuterX - *g_renderRows.rightX) >> 1) + *g_renderRows.rightOuterX,
                              *g_renderRows.y, i + 5, 1, SCENERY_TREE_3);
            if (w36 & 2)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_POLE);
        }
        if (w2 & ROADSIDE_TREE) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_TREE);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, SCENERY_TREE);
        }
        if (w2 & ROADSIDE_BUSH) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_BUSH);
            else if (w2 & ROADSIDE_BUSH_OUTER)
                render_object((*g_renderRows.rightOuterX << 1) - *g_renderRows.rightX, *g_renderRows.y, i + 5, 1,
                              SCENERY_BUSH);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, SCENERY_BUSH);
        }
        if (w2 & ROADSIDE_S_CURVE)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_S_CURVE_SIGN);
        if (w2 & ROADSIDE_S_CURVE_2)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_S_CURVE_SIGN_2);
        if (w2 & ROADSIDE_WARNING)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_WARNING_SIGN);
        if (w2 & ROADSIDE_BARRICADE)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, -1, SCENERY_BARRICADE);
        if (w36 & 0x100)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_SPEED_SIGN);
        if (w36 & 0x200)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_CHEVRON_LEFT);
        if (w36 & 0x400)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_CHEVRON_RIGHT);
        if (w36 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, SCENERY_NARROW_SIGN);
    next:
        if (g_trackViewMotion.roadIndex - g_roadOppPos.roadIndex == i && *(int *)&g_runRacingFlag) {
            en_end();
            render_opponent();
            en_begin();
        }
        g_renderRows.prevLeftX--;
        g_renderRows.prevRightX--;
        g_renderRows.prevY--;
        g_renderRows.leftX--;
        g_renderRows.rightX--;
        g_renderRows.y--;
        g_renderRows.lineWidth--;
        g_renderRows.leftFlags++;
        g_renderRows.rightFlags++;
        g_viewTerrainBitsData++;
        g_renderRows.leftOuterX--;
        g_renderRows.rightOuterX--;
        g_renderRows.buildingY--;
        g_renderRows.prevLeftOuterX--;
        g_renderRows.prevRightOuterX--;
        g_renderRows.prevBuildingY--;
    }
    vw_clip();
    clip_vline(2, 0xf0, 0x17, 0x2a);
    clip_vline(4, 0xf1, 0x17, 0x2a);
    clip_vline(4, 0x13e, 0x17, 0x2a);
    clip_vline(2, (SCREEN_WIDTH - 1), 0x17, 0x2a);
    pkt1.w = 0x20;
    pkt1.h = g_imageFullMirror->height - 6;
    pkt1.sx = 0xf8;
    pkt1.sy = 0x16;
    pkt1.dx = 8;
    pkt1.dy = (r_vid.mode == VID_EGA ? 0 : DASH_Y) + 3;
    (*g_videoOps.copy)(g_viewPages.draw == 1 ? g_driveScreenB : g_bufferSceneA, g_driveDisplayPage, &pkt1, 1);
}


void vw_draw(void)
{
    int w36;
    enum RoadsideFlags w11;
    int i;
    int f3;
    enum RoadsideFlags w2;

    g_renderRows.leftX = &g_roadLXArray[14];
    g_renderRows.rightX = &g_rightXProjectionTable[14];
    g_renderRows.y = &g_roadYProjectionArray[14];
    g_renderRows.leftOuterX = &g_roadOuterLeftXArray[14];
    g_renderRows.rightOuterX = &g_rhsOuterXArray[14];
    g_renderRows.buildingY = &g_yBuildingTopArray[14];
    g_renderRows.prevLeftX = &g_roadLXArray[13];
    g_renderRows.prevRightX = &g_rightXProjectionTable[13];
    g_renderRows.prevY = &g_roadYProjectionArray[13];
    g_renderRows.prevBuildingY = &g_yBuildingTopArray[13];
    g_renderRows.prevLeftOuterX = &g_roadOuterLeftXArray[13];
    g_renderRows.prevRightOuterX = &g_rhsOuterXArray[13];
    g_renderRows.lineWidth = &g_roadLineWidths[14];
    g_renderRows.leftFlags = (int far *)(g_roadTerrain + 14);
    g_renderRows.rightFlags = (int far *)(g_rightSideTerrainRow + 14);
    g_viewTerrainBitsData = (int far *)(g_hereFlagsPtr + 14);
    vw_clip();
    render_backdrop();
    en_begin();
    for (i = 14; i; i--) {
        if (i <= g_roadRowLimit)
            render_row(i);
        if (*g_viewTerrainBitsData & 4)
            goto next;
        if (i >= 11)
            goto next;
        f3 = *g_viewTerrainBitsData & 1;
        w11 = *g_renderRows.leftFlags;
        w2 = *g_renderRows.rightFlags;
        w36 = *g_viewTerrainBitsData;
        if (w11 & ROADSIDE_TREE) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, SCENERY_TREE);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, SCENERY_TREE);
        }
        if (w11 & ROADSIDE_BUSH) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, SCENERY_BUSH);
            else if (w11 & ROADSIDE_BUSH_OUTER)
                render_object(*g_renderRows.leftOuterX - *g_renderRows.leftX + *g_renderRows.leftOuterX,
                              *g_renderRows.y, i, -1, SCENERY_BUSH);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, SCENERY_BUSH);
        }
        if (w11 & ROADSIDE_WARNING)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, SCENERY_WARNING_SIGN);
        if (w11 & ROADSIDE_BARRICADE)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, SCENERY_BARRICADE);
        if (w11 & ROADSIDE_POST) {
            if (f3) {
                en_end();
                render_scaled_pic(*g_renderRows.leftX, (*g_renderRows.buildingY + *g_renderRows.y) >> 1, i, 1, 0, 1);
                en_begin();
            } else
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, SCENERY_POST);
        }
        if (f3) {
            if (w11 & 0x40)
                render_hydrant(*g_renderRows.leftX, *g_renderRows.y, i, 1);
            if (w2 & 0x40)
                render_hydrant(*g_renderRows.rightX, *g_renderRows.y, i, -1);
        } else {
            if (w11 & ROADSIDE_SMALL_ROCK)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, SCENERY_SMALL_ROCK);
            if (w11 & ROADSIDE_LARGE_ROCK)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, SCENERY_LARGE_ROCK);
            if (w11 & 0x40)
                render_object(-(((*g_renderRows.leftX - *g_renderRows.leftOuterX) >> 1) - *g_renderRows.leftOuterX),
                              *g_renderRows.y, i, -1, SCENERY_TREE_2);
            if (w11 & ROADSIDE_TREE_3)
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, SCENERY_TREE_3);
            if (w2 & ROADSIDE_SMALL_ROCK)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_SMALL_ROCK);
            if (w2 & ROADSIDE_LARGE_ROCK)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_LARGE_ROCK);
            if (w2 & 0x40)
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, SCENERY_TREE_2);
            if (w2 & ROADSIDE_TREE_3)
                render_object(((*g_renderRows.rightOuterX - *g_renderRows.rightX) >> 1) + *g_renderRows.rightOuterX,
                              *g_renderRows.y, i, 1, SCENERY_TREE_3);
            if (w36 & 2)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_POLE);
        }
        if (w2 & ROADSIDE_TREE) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_TREE);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, SCENERY_TREE);
        }
        if (w2 & ROADSIDE_BUSH) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_BUSH);
            else if (w2 & ROADSIDE_BUSH_OUTER)
                render_object((*g_renderRows.rightOuterX << 1) - *g_renderRows.rightX, *g_renderRows.y, i, 1,
                              SCENERY_BUSH);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, SCENERY_BUSH);
        }
        if (w2 & ROADSIDE_S_CURVE)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_S_CURVE_SIGN);
        if (w2 & ROADSIDE_S_CURVE_2)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_S_CURVE_SIGN_2);
        if (w2 & ROADSIDE_WARNING)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_WARNING_SIGN);
        if (w2 & ROADSIDE_BARRICADE)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, -1, SCENERY_BARRICADE);
        if (w2 & ROADSIDE_POST) {
            if (f3) {
                en_end();
                render_scaled_pic(*g_renderRows.rightX, (*g_renderRows.buildingY + *g_renderRows.y) >> 1, i, -1, 0, 1);
                en_begin();
            } else
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_POST);
        }
        if (w36 & 0xf000)
            render_gantry(*g_renderRows.leftX, *g_renderRows.rightX,
                          -((*g_renderRows.y - *g_renderRows.buildingY) * 4 - *g_renderRows.y), *g_renderRows.y, i, 0,
                          w36 & 0xf000);
        if (w36 & 0x100)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_SPEED_SIGN);
        if (w36 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_NARROW_SIGN);
        if (w36 & 0x200)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_CHEVRON_LEFT);
        if (w36 & 0x400)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, SCENERY_CHEVRON_RIGHT);
    next:
        if (g_roadOppPos.roadIndex - g_trackViewMotion.roadIndex == i && *(int *)&g_runRacingFlag) {
            en_end();
            render_opponent();
            en_begin();
        }
        g_renderRows.prevLeftX--;
        g_renderRows.prevRightX--;
        g_renderRows.prevY--;
        g_renderRows.leftX--;
        g_renderRows.rightX--;
        g_renderRows.y--;
        g_renderRows.lineWidth--;
        g_renderRows.leftFlags--;
        g_renderRows.rightFlags--;
        g_viewTerrainBitsData--;
        g_renderRows.leftOuterX--;
        g_renderRows.rightOuterX--;
        g_renderRows.buildingY--;
        g_renderRows.prevLeftOuterX--;
        g_renderRows.prevRightOuterX--;
        g_renderRows.prevBuildingY--;
    }
    if (g_roadOppPos.roadIndex == g_trackViewMotion.roadIndex && *(int *)&g_runRacingFlag) {
        en_end();
        render_opponent();
        en_begin();
    }
    render_mirror();
    en_end();
    render_overlays();
    if (*(int *)&g_runRacingFlag)
        render_start_signal();
    flip_page();
}


void vw_start(void)
{
    g_picOpponent = g_prevRivalFrame = 0;
    select_draw_page();
    rd_draw_frame();
}
