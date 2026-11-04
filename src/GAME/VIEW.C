#include <stdlib.h>
#include "STRUCTS.H"
#include "VIEW.H"
#include "ARENA.H"
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

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_6e04aay;                      /* DS:6E04 */
static int g_6e06aax;                      /* DS:6E06 */
static struct RenderRows g_renderRows; /* DS:6E08 */
static int g_oldF;                        /* DS:6E2A */
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
static void near render_object(int x, int y, int n, int f, int idx);
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

int g_opponentPics[5][4] = {{1003, 1004, 1057, 1091}, {1010, 1012, 1011, 1239}, {1010, 1247, 1248, 1092}, {1010, 1249, 1250, 1251}, {1010, 1253, 1252, 1254}};   /* DS:5E14 */
int g_5e3c = 0;   /* DS:5E3C */
struct ViewPages g_viewPages = {1, 2}; /* DS:5E3E */
struct Clip g_viewRect = {SCREEN_WIDTH, 82, 0, 18, 0, 18};   /* DS:5E42 */
int g_clipXMin = 0;   /* DS:5E4E */
int g_clipYMin = 18;   /* DS:5E50 */
int g_clipYMax = 101;   /* DS:5E52 */
int g_clipXMax = (SCREEN_WIDTH - 1);   /* DS:5E54 */
unsigned char g_shrinkMasks[3][4] = {{87, 37, 34, 0}, {247, 117, 82, 0}, {117, 84, 33, 0}};   /* DS:5E56 */
int g_5e62[18] = {1258, 1237, 1236, 1238, 8, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0};   /* DS:5E62 */
int g_5e86 = 0;   /* DS:5E86 */
int g_5e88[5] = {0, 0, 0, 0, 0};   /* DS:5E88 */
int g_5e92 = 0;   /* DS:5E92 */
int g_5e94[4] = {0, 0, 0, 0};   /* DS:5E94 */
struct Ball g_sheetRects[7] = {{256, 8, 8}, {240, 16, 13}, {208, 32, 19}, {168, 40, 24}, {120, 48, 30}, {64, 56, 37}, {0, 64, 43}};   /* DS:5E9C */

/* select_draw_page @ 2645:224E (linear 0x2869E, 58 bytes) */

static void near select_draw_page(void)
{
    if (r_vid.mode == VID_EGA) {
        av787at = (g_viewPages.draw == 1) ? 0xa200 : 0xa400;
        egapage_show(g_viewPages.show);
    } else {
        av787at = g8188w->bits.w.seg;
        at767ct = g8188w->bits.w.off;
    }
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int a7312av;
struct Bitmap far *k_6e60;
int dz70e0;
struct Bitmap far *ax72f4;
int br7314;
struct Bitmap far *g764aw;
struct Bitmap far *ar767eu;
int g789eu;
int f_78a0;
int ac7ae8t;
int az7aeau;
int e_7f2e;
int g7f30t;
int al7fa4z;
int by7fa6;
int dz7ff2;
int g7ff4g;
struct Bitmap far *g8184y;
struct Bitmap far *bz8b7ex;
int far *am8b82y;
struct Bitmap far *ak8bcat;

/* flip_page @ 2645:2288 (linear 0x286D8, 116 bytes) */

static void near flip_page(void)
{
    int t;

    if (r_vid.mode == VID_EGA) {
        t = g_viewPages.draw;
        g_viewPages.draw = g_viewPages.show;
        g_viewPages.show = t;
        av787at = (g_viewPages.draw == 1) ? 0xa200 : 0xa400;
        egapage_show(g_viewPages.show);
    } else {
        (*g_videoOps.copy)(g8188w, r_vid.front, (struct Blt *)&g_viewRect, 1);
        av787at = g8188w->bits.w.seg;
        at767ct = g8188w->bits.w.off;
    }
}

/* clip_blit_y @ 2645:22FC (linear 0x2874C, 53 bytes)

   Clips the vertical extent of a blit rectangle to the clip window. */

static void near clip_blit_y(struct Blt *b)
{
    if (b->dy < g_clipYMin) {
        b->h -= g_clipYMin - b->dy;
        b->sy += g_clipYMin - b->dy;
        b->dy = g_clipYMin;
    }
    if (g_clipYMax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clipYMax;
}

/* clip_blit @ 2645:2331 (linear 0x28781, 80 bytes)

   Clips a blit rectangle to the clip window, vertically and then
   horizontally. */

static void near clip_blit(struct Blt *b)
{
    if (b->dy < g_clipYMin) {
        b->h -= g_clipYMin - b->dy;
        b->sy += g_clipYMin - b->dy;
        b->dy = g_clipYMin;
    }
    if (g_clipYMax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clipYMax;
    if (b->dx < g_clipXMin) {
        b->w -= g_clipXMin - b->dx;
        b->sx += g_clipXMin - b->dx + 7;
        b->dx = g_clipXMin;
    }
}

/* clip_vline @ 2645:2381 (linear 0x287D1, 111 bytes)

   Draws a clipped vertical line (color, x, y1, y2). */

static void near clip_vline(int color, int x, int y1, int y2)
{
    if (y2 >= g_clipYMin && y1 <= g_clipYMax && x >= g_clipXMin && x <= g_clipXMax) {
        y1 = (y1 < g_clipYMin) ? g_clipYMin : y1;
        y1 = (y1 > g_clipYMax) ? g_clipYMax : y1;
        y2 = (y2 < g_clipYMin) ? g_clipYMin : y2;
        y2 = (y2 > g_clipYMax) ? g_clipYMax : y2;
        (*g_videoOps.drawLine)(x, y1, x, y2, color);
    }
}

/* view_unclip_top @ 2645:23F0 (linear 0x28840, 7 bytes) */

void view_unclip_top(void)
{
    g_clipYMin = 0;
}

/* clip_mirror @ 2645:23F7 (linear 0x28847, 25 bytes)

   Sets the clip window to x 242..317, y 22..42. */

static void near clip_mirror(void)
{
    g_clipXMin = 0xf2;
    g_clipYMin = 0x16;
    g_clipXMax = 0x13d;
    g_clipYMax = 0x2a;
}

/* view_clip @ 2645:2410 (linear 0x28860, 25 bytes)

   Sets the clip window to x 0..319, y 18..101. */

void view_clip(void)
{
    g_clipXMin = 0;
    g_clipYMin = 0x12;
    g_clipYMax = 0x65;
    g_clipXMax = (SCREEN_WIDTH - 1);
}

/* view_load @ 2645:2429 (linear 0x28879, 1369 bytes) */

void view_load(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *(far *load)(struct Bitmap far *, unsigned char, unsigned char);
    struct Blt pkt1;
    int sel;
    struct Bitmap far *bm24;
    int il;
    int idx;

    marks_clear();
    bm1 = F_15BBE(0x3ed, ARENA_HIGH_STACK);
    pkt1.w = bm1->width;
    pkt1.h = bm1->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_videoOps.copy)(bm1, g8188w, &pkt1, 1);
    F_19438(1);
    sel = *(int *)&bw8aca == 0 ? g_scene : 0;
    bm24 = F_15BBE(g_5e62[sel], ARENA_HIGH_STACK);
    pkt1.w = bm24->width;
    pkt1.h = bm24->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_videoOps.copy)(bm24, g82b4s, &pkt1, 1);
    F_19438(1);
    if (*(int *)&bw8aca == 0) {
        bm24 = F_15BBE(0x4ed, ARENA_HIGH_STACK);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 1;
        (*g_videoOps.copy)(bm24, r_vid.mode == VID_EGA ? bm7678 : g82b4s, &pkt1, 1);
        F_19438(1);
        bm24 = F_15BBE(0x4ee, ARENA_HIGH_STACK);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (r_vid.mode == VID_EGA ? DASH_Y - 1 : 0) + 0x2c;
        (*g_videoOps.copy)(bm24, r_vid.mode == VID_EGA ? bm7678 : g82b4s, &pkt1, 1);
        F_19438(1);
    }
    ar767eu = F_15BBE(0x4f3, ARENA_LOW_STACK);
    if (*(int *)&bw8aca == 0) {
        g764aw = F_15BBE(0x3f0, ARENA_LOW_STACK);
        pkt1.w = g764aw->width;
        pkt1.h = g764aw->height;
        pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x2c;
        pkt1.dy = 0xc;
        (*g_videoOps.maskedBlit)(g764aw, ar767eu, &pkt1, 1);
        marks_pop_low(1);
    }
    pkt1.w = ar767eu->width;
    pkt1.h = ar767eu->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = SCREEN_HEIGHT - ar767eu->height;
    (*g_videoOps.copy)(ar767eu, g8188w, &pkt1, 1);
    marks_pop_low(1);
    ax72f4 = F_15BBE(0x3ee, ARENA_LOW_STACK);
    ak8bcat = F_15BBE(0x3ef, ARENA_LOW_STACK);
    g8184y = F_15BBE(0x3f1, ARENA_LOW_STACK);
    bz8b7ex = F_15BBE(0x4db, ARENA_LOW_STACK);
    if (*(int *)&bw8aca) {
        idx = w_8df0[bn7648].kind;
        k_6e60 = F_15BBE(0x3f5, ARENA_LOW_STACK);
        aw7aaex = k_6e60->height;
        g8acew[0] = F_15BBE(g_opponentPics[idx][0], ARENA_LOW_STACK);
        g8acew[1] = F_15BBE(g_opponentPics[idx][1], ARENA_LOW_STACK);
        g8acew[2] = F_15BBE(g_opponentPics[idx][2], ARENA_HIGH_STACK);
        load = r_vid.mode == VID_EGA ? shrink_ega : r_vid.mode == VID_TANDY ? shrink_tandy : shrink_cga;
        for (il = 0; il < 3; il++)
            g8acew[3 + il] = (*load)(g8acew[2], g_shrinkMasks[0][il], g_shrinkMasks[0][il]);
        for (il = 0; il < 3; il++)
            g8acew[6 + il] = (*load)(g8acew[5], g_shrinkMasks[1][il], g_shrinkMasks[1][il]);
        g8acew[9] = F_15BBE(g_opponentPics[idx][3], ARENA_LOW_STACK);
        for (il = 0; il < 3; il++)
            g8acew[10 + il] = (*load)(g8acew[9], g_shrinkMasks[2][il], g_shrinkMasks[2][il]);
        ab7526x[0] = g8acew[0];
        ab7526x[1] = F_15BBE(0x4d9, ARENA_LOW_STACK);
        ab7526x[2] = F_15BBE(0x4da, ARENA_HIGH_STACK);
        ab7526x[2] = ab7526x[3] = (*load)(ab7526x[2], g_shrinkMasks[0][0], g_shrinkMasks[0][0]);
        F_19438(1);
        for (il = 0; il < 3; il++)
            ab7526x[4 + il] = ab7526x[3];
        ab7526x[9] = F_15BBE(0x4de, ARENA_LOW_STACK);
        for (il = 0; il < 3; il++)
            ab7526x[10 + il] = (*load)(ab7526x[9], g_shrinkMasks[2][il], g_shrinkMasks[2][il]);
    }
}

/* render_backdrop @ 2645:2982 (linear 0x28DD2, 648 bytes) */

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
    if (*(int *)&bw8aca) {
        v = road_proj_y(n8, m5 * 2, ao7d2ev.m7d3e);
        b.h = v - 0x1e;
        b.sy = 0x9a - b.h;
        if (ao7d2ev.m7d3c > 0) {
            r = (ao7d2ev.m7d3c * 2 % SCREEN_WIDTH) & ~7;
            ll = SCREEN_WIDTH - r;
        } else {
            ll = (abs(ao7d2ev.m7d3c) * 2 % SCREEN_WIDTH) & ~7;
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
    (*g_videoOps.copy)(g8188w, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    if (*(int *)&bw8aca) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_videoOps.copy)(g8188w, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    }
    b.h = *(int *)&bw8aca ? 0x14 : 0x10;
    b.sy = 0xa3;
    if (*(int *)&bw8aca) {
        if (ao7d2ev.m7d3c > 0) {
            r = (ao7d2ev.m7d3c * 4 % SCREEN_WIDTH) & ~7;
            ll = SCREEN_WIDTH - r;
        } else {
            ll = (abs(ao7d2ev.m7d3c) * 4 % SCREEN_WIDTH) & ~7;
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
    (*g_videoOps.copy)(g8188w, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    g_6e06aax = 14;
    if (*(int *)&bw8aca) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_videoOps.copy)(g8188w, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
        g_6e04aay = v + 8;
        for (i = 14; i; i--) {
            if (bq7884[i] < g_6e04aay) {
                g_6e04aay = bq7884[i];
                g_6e06aax = i;
            }
        }
        (*g_videoOps.fillBelow)(g_6e04aay, 9);
    } else
        (*g_videoOps.fillBelow)(0x3f, 1);
}

/* view_sprite @ 2645:2C0A (linear 0x2905A, 242 bytes) */

void view_sprite(int x, int y, int first, struct List l, int dir)
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

/* edge_step @ 2645:2CFC (linear 0x2914C, 36 bytes) */

static int near edge_step(int x1, int y1, int x2, int y2)
{
    if (y2 == y1)
        return 0;
    return ((x2 - x1) << 4) / (y2 - y1);
}

/* fill_quad @ 2645:2D20 (linear 0x29170, 523 bytes) */

static void near fill_quad(int col, int a, int b, int c, int d, int e, int f)
{
    int mode;
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
            mode = 2;
            xlk = a;
            racc6 = a << 4;
        } else if (e < f && d > c) {
            mode = 3;
            xr5 = b;
            xlk = b << 4;
        } else {
            if (f <= e && c <= d) {
                mode = 4;
                racc6 = a << 4;
            } else {
                mode = 5;
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
        case 2:
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else if (y6 <= d)
                xr5 = b;
            else
                xr5 = (racc6 += dxb) >> 4;
            break;
        case 3:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            break;
        case 4:
            if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else
                xr5 = b;
            break;
        case 5:
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
        if (y6 <= ymax && y6 <= g_clipYMax)
            goto loop;
    }
}

/* sgn @ 2645:2F2B (linear 0x2937B, 29 bytes) */

static int near sgn(int n)
{
    if (n > 0)
        return 1;
    if (n < 0)
        return -1;
    return 0;
}

/* view_road_row @ 2645:2F48 (linear 0x29398, 773 bytes) */

void view_road_row(int n)
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
    flagk = *am8b82y & 1;
    if (flagk) {
        xl2 = *g_renderRows.leftOuterX;
        xr28 = *g_renderRows.rightOuterX;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = edge_step(*g_renderRows.prevLeftOuterX, ybot6, xl2, yk);
        dxr26 = edge_step(*g_renderRows.prevRightOuterX, ybot6, xr28, yk);
    }
    if (*am8b82y & 8) {
        wid = *g_renderRows.lineWidth;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (sgn(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 101; cnt--, yk++) {
        if (flagk) {
            if (*am8b82y & 4)
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
            (*g_videoOps.fillSpan)(xl, xrl, yk, (*am8b82y & 0x10) ? 15 : 1);
        }
        if (*am8b82y & 8) {
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

/* view_mirror_row @ 2645:324D (linear 0x2969D, 653 bytes) */

void view_mirror_row(void)
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
    flagk = *am8b82y & 1;
    if (flagk) {
        xl2 = *g_renderRows.leftOuterX;
        xr28 = *g_renderRows.rightOuterX;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = edge_step(*g_renderRows.prevLeftOuterX, ybot6, xl2, yk);
        dxr26 = edge_step(*g_renderRows.prevRightOuterX, ybot6, xr28, yk);
    }
    if (*am8b82y & 8) {
        wid = *g_renderRows.lineWidth;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (sgn(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 42; cnt--, yk++) {
        if (flagk) {
            if (*am8b82y & 4)
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
        if (*am8b82y & 8) {
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

/* render_buildings @ 2645:34DA (linear 0x2992A, 725 bytes) */

static void near render_buildings(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b8l;
    int b4;
    int t8l;
    int y;

    t8l = *am8b82y & 1;
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
    if (!(*am8b82y & 4)) {
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
        if (am8b82y[-1] & 4) {
            if (rev)
                (*g_videoOps.fillSpan)(g_clipXMin, a, c - 1, 4);
            else
                (*g_videoOps.fillSpan)(b, g_clipXMax, d - 1, 4);
            if (rev) {
                for (y = f; y < c - 1; y++)
                    (*g_videoOps.fillSpan)(g_clipXMin, a, y, 2);
            } else {
                for (y = e; y < d - 1; y++)
                    (*g_videoOps.fillSpan)(b, g_clipXMax, y, 2);
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

/* render_mirror_buildings @ 2645:37AF (linear 0x29BFF, 559 bytes) */

static void near render_mirror_buildings(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b4;
    int t8l;
    int y;

    if (t8l = *am8b82y & 1) {
        if (rev) {
            a41 = g_renderRows.leftFlags[0] & 4;
            b4 = g_renderRows.leftFlags[1] & 4;
            c10 = g_renderRows.leftFlags[0] & 0x10;
        } else {
            a41 = g_renderRows.rightFlags[0] & 4;
            b4 = g_renderRows.rightFlags[1] & 4;
            c10 = g_renderRows.rightFlags[0] & 0x10;
        }
        if (!(*am8b82y & 4)) {
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
            if (am8b82y[1] & 4) {
                if (rev)
                    (*g_videoOps.fillSpan)(g_clipXMin, a, c - 1, 4);
                else
                    (*g_videoOps.fillSpan)(b, g_clipXMax, d - 1, 4);
                if (rev) {
                    for (y = f; y < c - 1; y++)
                        (*g_videoOps.fillSpan)(g_clipXMin, a, y, 2);
                } else {
                    for (y = e; y < d - 1; y++)
                        (*g_videoOps.fillSpan)(b, g_clipXMax, y, 2);
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

/* render_left_buildings @ 2645:39DE (linear 0x29E2E, 47 bytes) */

static void near render_left_buildings(void)
{
    render_buildings(*g_renderRows.prevLeftOuterX, *g_renderRows.leftOuterX, *g_renderRows.prevY, *g_renderRows.y, *g_renderRows.buildingY, *g_renderRows.prevBuildingY, 1);
}

/* render_right_buildings @ 2645:3A0D (linear 0x29E5D, 46 bytes) */

static void near render_right_buildings(void)
{
    render_buildings(*g_renderRows.rightOuterX, *g_renderRows.prevRightOuterX, *g_renderRows.y, *g_renderRows.prevY, *g_renderRows.prevBuildingY, *g_renderRows.buildingY, 0);
}

/* render_row @ 2645:3A3B (linear 0x29E8B, 28 bytes) */

static void near render_row(int n)
{
    if (*(int *)&bw8aca == 0) {
        render_left_buildings();
        render_right_buildings();
    }
    view_road_row(n);
}

/* render_mirror_left @ 2645:3A57 (linear 0x29EA7, 47 bytes) */

static void near render_mirror_left(void)
{
    render_mirror_buildings(*g_renderRows.prevLeftOuterX, *g_renderRows.leftOuterX, *g_renderRows.prevY, *g_renderRows.y, *g_renderRows.buildingY, *g_renderRows.prevBuildingY, 1);
}

/* render_mirror_right @ 2645:3A86 (linear 0x29ED6, 46 bytes) */

static void near render_mirror_right(void)
{
    render_mirror_buildings(*g_renderRows.rightOuterX, *g_renderRows.prevRightOuterX, *g_renderRows.y, *g_renderRows.prevY, *g_renderRows.prevBuildingY, *g_renderRows.buildingY, 0);
}

/* render_mirror_row @ 2645:3AB4 (linear 0x29F04, 11 bytes) */

static void near render_mirror_row(void)
{
    render_mirror_left();
    render_mirror_right();
    view_mirror_row();
}

/* view_draw_dot @ 2645:3ABF (linear 0x29F0F, 44 bytes) */

void view_draw_dot(int x, int y)
{
    egaspan_begin();
    view_sprite(x, y, 0, *g_scenery[20], 1);
    egaspan_end();
}

/* render_object @ 2645:3AEB (linear 0x29F3B, 79 bytes) */

static void near render_object(int x, int y, int n, int f, int idx)
{
    n = 9 - n;
    if (n >= 7)
        n = 6;
    if (n < 0)
        n = 0;
    if (y > 0x65)
        y = 0x65;
    view_sprite(x, y, n, *g_scenery[idx], f);
}

/* render_hydrant @ 2645:3B3A (linear 0x29F8A, 63 bytes) */

static void near render_hydrant(int x, int y, int n, int f)
{
    n = 9 - n;
    if (n >= 8)
        n = 7;
    if (n < 0)
        n = 0;
    view_sprite(x, y, n, *g_scenery[2], f);
}

/* render_scaled_pic @ 2645:3B79 (linear 0x29FC9, 339 bytes) */

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
        (*g_videoOps.copy)(g82b4s, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    else
        (*g_videoOps.copy)(r_vid.mode == VID_EGA ? bm7678 : g82b4s, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
}

/* render_gantry @ 2645:3CCC (linear 0x2A11C, 247 bytes) */

static void near render_gantry(int x1, int x2, int y1, int y2, int n, int clip, int flags)
{
    int j;

    for (j = *g_renderRows.lineWidth + 1; j; j--) {
        egaspan_end();
        clip_vline(10, x1 - j, y1, y2);
        clip_vline(10, x2 + j, y1, y2);
        egaspan_begin();
        (*g_videoOps.fillSpan)(x1, x2, y1 + j - 1, 10);
    }
    if (flags & 0x1000)
        render_object((x1 + x2) >> 1, y1, n, 1, 13);
    else if (flags & 0x2000) {
        egaspan_end();
        render_scaled_pic((x1 + x2) >> 1, y1, n, 0, clip, 1);
        egaspan_begin();
    } else if (*am8b82y & 1) {
        egaspan_end();
        render_scaled_pic((x1 + x2) >> 1, y1, n, 0, clip, (flags & 0x4000) ? 2 : 3);
        egaspan_begin();
    } else
        render_object((x1 + x2) >> 1, y1, n, 1, (flags & 0x4000) ? 12 : 14);
}

/* render_start_signal @ 2645:3DC3 (linear 0x2A213, 121 bytes) */

static void near render_start_signal(void)
{
    struct Blt b;

    if (aw7aaex > 0) {
        b.w = 0x18;
        b.h = aw7aaex;
        if (g7d66n) {
            ao8228u = 1;
            b.sx = 0;
        } else
            b.sx = 0x18;
        b.sy = 0x20 - aw7aaex;
        b.dx = 0x82;
        b.dy = 0x12;
        (*g_videoOps.maskedBlit)(k_6e60, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    }
}

/* render_overlays @ 2645:3E3C (linear 0x2A28C, 159 bytes) */

static void near render_overlays(void)
{
    struct Blt b;

    b.w = ax72f4->width;
    b.h = ax72f4->height;
    b.dx = b.sx = b.sy = 0;
    b.dy = 0x12;
    (*g_videoOps.maskedBlit)(ax72f4, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    b.w = ak8bcat->width;
    b.h = ak8bcat->height;
    b.sx = b.sy = 0;
    b.dx = 0x58;
    b.dy = 0x5f;
    (*g_videoOps.maskedBlit)(ak8bcat, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
}

/* render_mirror_frame @ 2645:3EDB (linear 0x2A32B, 178 bytes) */

static void near render_mirror_frame(void)
{
    struct Blt b;

    b.w = g8184y->width;
    b.h = g8184y->height;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = 0x12;
    (*g_videoOps.copy)(g8184y, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
    b.w = bz8b7ex->width;
    b.h = 2;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = g_mirYProj[7] - 2;
    clip_blit_y(&b);
    (*g_videoOps.copy)(bz8b7ex, g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
}

/* render_opponent @ 2645:3F8D (linear 0x2A3DD, 1136 bytes) */

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

    a2 = ar7e8av.roadIndex;
    xk = (a2 - ao7d2ev.roadIndex) * 8 + ar7e8av.roadFraction - ao7d2ev.roadFraction;
    z9 = ar7e8av.lateralX - ao7d2ev.lateralX;
    if (xk >= 0x78)
        seln = -1;
    else if (xk >= 0x10) {
        if ((seln = ((xk + 8) >> 4) + 2) > 8)
            seln = 8;
    } else if (xk < -7)
        seln = ((0x10 - xk) >> 4) + 8;
    else if (xk <= 8) {
        if (abs(z9) < 180) {
            if (ap7d7cv <= 8)
                seln = 2;
            else
                seln = 9;
        } else
            seln = z9 < 0 ? 0 : 2;
    } else if (z9 < -180)
        seln = 1;
    else
        seln = 3;
    if (bu8acc >= 4 && seln != ap7d7cv) {
        if (seln < 0) {
            if (ap7d7cv == 8)
                return;
            seln = ap7d7cv + 1;
        } else if (seln > 12) {
            if (ap7d7cv == 12)
                return;
            seln = ap7d7cv + 1;
        } else if (seln != g_oldF) {
            if ((seln <= 8 && ap7d7cv > 9) || (seln >= 9 && ap7d7cv <= 8 && ap7d7cv > 0))
                seln = ap7d7cv - 1;
            else if (ap7d7cv == 0)
                seln = seln < 9 ? 1 : 9;
            else if (ap7d7cv == 9)
                seln = seln < 9 ? (z9 < 0 ? 0 : 2) : 10;
            else if (ap7d7cv == 1)
                seln = seln < 1 ? 0 : 3;
            else if (ap7d7cv == 3 && seln == 1)
                ;
            else if (seln > ap7d7cv)
                seln = ap7d7cv + 1;
            else
                seln = ap7d7cv - 1;
        }
    }
    ay8bc2z = seln == 0 || (seln == 2 && xk <= 10);
    by72f2 = seln == 2 && xk <= 10 && abs(z9) < 180 &&
        (bu8acc < 4 ? 100 : 180) > abs(z9) && ar7e8av.distanceStep < ao7d2ev.distanceStep;
    g_oldF = ap7d7cv;
    ap7d7cv = seln;
    g_5ec6 = xk > 16 ? (xk + 8) & 15 : 0;
    t1 = xk;
    if (seln == 0)
        t1 += 16;
    if (t1 < -2 && seln == 2)
        t1 += 8;
    rc = ao7d2ev.m7d3e - g_terrain4[a2];
    rd = ao7d2ev.m7d3c - g_terrain2[a2];
    if (seln < 9) {
        t1 += 4;
        ra = road_proj_x(t1, ar7e8av.lateralX, rd);
        rb = road_proj_y(t1, 2000, rc);
    } else {
        t1 = -t1;
        ra = road_mirror_x(t1, ar7e8av.lateralX, -rd);
        rb = road_mirror_y(t1, 2000, -rc);
    }
    if (seln == 0) {
        bm = g8acew[seln];
        b.w = bm->width;
        b.h = bm->height;
        if ((b.sx = b.w - ra + 0x40) < 0)
            b.sx = 0;
        b.dx = 0;
        b.sy = 0;
        b.dy = rb;
    } else if (seln == 1) {
        bm = g8acew[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - b.w;
        b.sy = 0;
        b.dy = rb - b.h + 0x11;
    } else {
        bm = g8acew[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - (b.w >> 1);
        b.sy = 0;
        b.dy = (seln < 9 ? (rb < g_6e04aay ? g_6e04aay : rb) : rb) - b.h;
        if (seln == 2 && b.dy + b.h >= 0x65)
            b.dy = 0x65 - b.h;
    }
    flag9 = 0;
    if (seln >= 9) {
        if (g_clipXMin == 0) {
            flag9 = 1;
            clip_mirror();
        }
    } else if (g_clipXMin != 0) {
        flag9 = 1;
        view_clip();
    }
    if (b.dx + b.w >= g_clipXMin && b.dx < g_clipXMax) {
        if (seln >= 9)
            clip_blit(&b);
        else
            clip_blit_y(&b);
        (*g_videoOps.maskedBlit)(g8acew[seln], g_viewPages.draw == 1 ? g8188w : g82b4s, &b, 1);
        if (flag9) {
            if (g_clipXMin == 0)
                clip_mirror();
            else
                view_clip();
        }
    }
}

/* render_mirror @ 2645:43FD (linear 0x2A84D, 1935 bytes) */

static void near render_mirror(void)
{
    int w36;
    struct Blt pkt1;
    int w11;
    int i;
    int f3;
    int w2;

    g_renderRows.leftX = &g7f30t;
    g_renderRows.rightX = &g_mirGeom.rightX[7];
    g_renderRows.y = &g_mirYProj[7];
    g_renderRows.leftOuterX = &al70c0x.m70d0;
    g_renderRows.rightOuterX = &dz70e0;
    g_renderRows.buildingY = &g_mirGeom.buildingY[7];
    g_renderRows.prevLeftX = &e_7f2e;
    g_renderRows.prevRightX = &g_mirGeom.rightX[6];
    g_renderRows.prevY = &g_mirYProj[6];
    g_renderRows.prevBuildingY = &g_mirGeom.buildingY[6];
    g_renderRows.prevLeftOuterX = &al70c0x.m70ce;
    g_renderRows.prevRightOuterX = &al70c0x.m70de;
    g_renderRows.lineWidth = &g_5e92;
    g_renderRows.leftFlags = (int far *)(q_72ee - 8);
    g_renderRows.rightFlags = (int far *)(g_rightSideTerrainRow - 8);
    am8b82y = (int far *)(g7d62p - 8);
    egaspan_end();
    render_mirror_frame();
    egaspan_begin();
    clip_mirror();
    for (i = 7; i; i--) {
        render_mirror_row();
        if (*am8b82y & 4)
            goto next;
        if (*am8b82y & 0xf000)
            render_gantry(*g_renderRows.leftX, *g_renderRows.rightX, -((*g_renderRows.y - *g_renderRows.buildingY) * 4 - *g_renderRows.y), *g_renderRows.y, i + 5, 1, *am8b82y & 0xf000);
        if (i >= 6)
            goto next;
        f3 = *am8b82y & 1;
        w11 = *g_renderRows.leftFlags;
        w2 = *g_renderRows.rightFlags;
        w36 = *am8b82y;
        if (w11 & 0x20) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, 0);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, 0);
        }
        if (w11 & 0x100) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, 4);
            else if (w11 & 0x200)
                render_object(*g_renderRows.leftOuterX - *g_renderRows.leftX + *g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, 4);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, 4);
        }
        if (w11 & 0x800)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, 3);
        if (w11 & 0x1000)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, 6);
        if (f3) {
            if (w11 & 0x40)
                render_hydrant(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1);
            if (w2 & 0x40)
                render_hydrant(*g_renderRows.rightX, *g_renderRows.y, i + 5, -1);
        } else {
            if (w11 & 1)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, 1, 1);
            if (w11 & 4)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, 7);
            if (w11 & 8)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i + 5, -1, 8);
            if (w11 & 0x40)
                render_object(-(((*g_renderRows.leftX - *g_renderRows.leftOuterX) >> 1) - *g_renderRows.leftOuterX), *g_renderRows.y, i + 5, -1, 10);
            if (w11 & 0x80)
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i + 5, -1, 11);
            if (w2 & 1)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 1);
            if (w2 & 4)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 7);
            if (w2 & 8)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 8);
            if (w2 & 0x40)
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, 10);
            if (w2 & 0x80)
                render_object(((*g_renderRows.rightOuterX - *g_renderRows.rightX) >> 1) + *g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, 11);
            if (w36 & 2)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 9);
        }
        if (w2 & 0x20) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, 0);
        }
        if (w2 & 0x100) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 4);
            else if (w2 & 0x200)
                render_object((*g_renderRows.rightOuterX << 1) - *g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 4);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i + 5, 1, 4);
        }
        if (w2 & 0x2000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0x12);
        if (w2 & 0x4000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0x13);
        if (w2 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 3);
        if (w2 & 0x1000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, -1, 6);
        if (w36 & 0x100)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 5);
        if (w36 & 0x200)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0x10);
        if (w36 & 0x400)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0x11);
        if (w36 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i + 5, 1, 0xf);
    next:
        if (ao7d2ev.roadIndex - ar7e8av.roadIndex == i && *(int *)&bw8aca) {
            egaspan_end();
            render_opponent();
            egaspan_begin();
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
        am8b82y++;
        g_renderRows.leftOuterX--;
        g_renderRows.rightOuterX--;
        g_renderRows.buildingY--;
        g_renderRows.prevLeftOuterX--;
        g_renderRows.prevRightOuterX--;
        g_renderRows.prevBuildingY--;
    }
    view_clip();
    clip_vline(2, 0xf0, 0x17, 0x2a);
    clip_vline(4, 0xf1, 0x17, 0x2a);
    clip_vline(4, 0x13e, 0x17, 0x2a);
    clip_vline(2, (SCREEN_WIDTH - 1), 0x17, 0x2a);
    pkt1.w = 0x20;
    pkt1.h = g8184y->height - 6;
    pkt1.sx = 0xf8;
    pkt1.sy = 0x16;
    pkt1.dx = 8;
    pkt1.dy = (r_vid.mode == VID_EGA ? 0 : DASH_Y) + 3;
    (*g_videoOps.copy)(g_viewPages.draw == 1 ? g8188w : g82b4s, bm7678, &pkt1, 1);
}

/* view_draw @ 2645:4B8C (linear 0x2AFDC, 1774 bytes) */

void view_draw(void)
{
    int w36;
    int w11;
    int i;
    int f3;
    int w2;

    g_renderRows.leftX = &br7314;
    g_renderRows.rightX = &az7aeau;
    g_renderRows.y = &f_78a0;
    g_renderRows.leftOuterX = &by7fa6;
    g_renderRows.rightOuterX = &g7ff4g;
    g_renderRows.buildingY = &g_yBuildingTopArray[14];
    g_renderRows.prevLeftX = &a7312av;
    g_renderRows.prevRightX = &ac7ae8t;
    g_renderRows.prevY = &g789eu;
    g_renderRows.prevBuildingY = &g_yBuildingTopArray[13];
    g_renderRows.prevLeftOuterX = &al7fa4z;
    g_renderRows.prevRightOuterX = &dz7ff2;
    g_renderRows.lineWidth = &g_5e86;
    g_renderRows.leftFlags = (int far *)(q_72ee + 14);
    g_renderRows.rightFlags = (int far *)(g_rightSideTerrainRow + 14);
    am8b82y = (int far *)(g7d62p + 14);
    view_clip();
    render_backdrop();
    egaspan_begin();
    for (i = 14; i; i--) {
        if (i <= g_6e06aax)
            render_row(i);
        if (*am8b82y & 4)
            goto next;
        if (i >= 11)
            goto next;
        f3 = *am8b82y & 1;
        w11 = *g_renderRows.leftFlags;
        w2 = *g_renderRows.rightFlags;
        w36 = *am8b82y;
        if (w11 & 0x20) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, 0);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, 0);
        }
        if (w11 & 0x100) {
            if (f3)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, 4);
            else if (w11 & 0x200)
                render_object(*g_renderRows.leftOuterX - *g_renderRows.leftX + *g_renderRows.leftOuterX, *g_renderRows.y, i, -1, 4);
            else
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, 4);
        }
        if (w11 & 0x800)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, 3);
        if (w11 & 0x1000)
            render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, 6);
        if (w11 & 1) {
            if (f3) {
                egaspan_end();
                render_scaled_pic(*g_renderRows.leftX, (*g_renderRows.buildingY + *g_renderRows.y) >> 1, i, 1, 0, 1);
                egaspan_begin();
            } else
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, 1, 1);
        }
        if (f3) {
            if (w11 & 0x40)
                render_hydrant(*g_renderRows.leftX, *g_renderRows.y, i, 1);
            if (w2 & 0x40)
                render_hydrant(*g_renderRows.rightX, *g_renderRows.y, i, -1);
        } else {
            if (w11 & 4)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, 7);
            if (w11 & 8)
                render_object(*g_renderRows.leftX, *g_renderRows.y, i, -1, 8);
            if (w11 & 0x40)
                render_object(-(((*g_renderRows.leftX - *g_renderRows.leftOuterX) >> 1) - *g_renderRows.leftOuterX), *g_renderRows.y, i, -1, 10);
            if (w11 & 0x80)
                render_object(*g_renderRows.leftOuterX, *g_renderRows.y, i, -1, 11);
            if (w2 & 4)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 7);
            if (w2 & 8)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 8);
            if (w2 & 0x40)
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, 10);
            if (w2 & 0x80)
                render_object(((*g_renderRows.rightOuterX - *g_renderRows.rightX) >> 1) + *g_renderRows.rightOuterX, *g_renderRows.y, i, 1, 11);
            if (w36 & 2)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 9);
        }
        if (w2 & 0x20) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, 0);
        }
        if (w2 & 0x100) {
            if (f3)
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 4);
            else if (w2 & 0x200)
                render_object((*g_renderRows.rightOuterX << 1) - *g_renderRows.rightX, *g_renderRows.y, i, 1, 4);
            else
                render_object(*g_renderRows.rightOuterX, *g_renderRows.y, i, 1, 4);
        }
        if (w2 & 0x2000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0x12);
        if (w2 & 0x4000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0x13);
        if (w2 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 3);
        if (w2 & 0x1000)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, -1, 6);
        if (w2 & 1) {
            if (f3) {
                egaspan_end();
                render_scaled_pic(*g_renderRows.rightX, (*g_renderRows.buildingY + *g_renderRows.y) >> 1, i, -1, 0, 1);
                egaspan_begin();
            } else
                render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 1);
        }
        if (w36 & 0xf000)
            render_gantry(*g_renderRows.leftX, *g_renderRows.rightX, -((*g_renderRows.y - *g_renderRows.buildingY) * 4 - *g_renderRows.y), *g_renderRows.y, i, 0, w36 & 0xf000);
        if (w36 & 0x100)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 5);
        if (w36 & 0x800)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0xf);
        if (w36 & 0x200)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0x10);
        if (w36 & 0x400)
            render_object(*g_renderRows.rightX, *g_renderRows.y, i, 1, 0x11);
    next:
        if (ar7e8av.roadIndex - ao7d2ev.roadIndex == i && *(int *)&bw8aca) {
            egaspan_end();
            render_opponent();
            egaspan_begin();
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
        am8b82y--;
        g_renderRows.leftOuterX--;
        g_renderRows.rightOuterX--;
        g_renderRows.buildingY--;
        g_renderRows.prevLeftOuterX--;
        g_renderRows.prevRightOuterX--;
        g_renderRows.prevBuildingY--;
    }
    if (ar7e8av.roadIndex == ao7d2ev.roadIndex && *(int *)&bw8aca) {
        egaspan_end();
        render_opponent();
        egaspan_begin();
    }
    render_mirror();
    egaspan_end();
    render_overlays();
    if (*(int *)&bw8aca)
        render_start_signal();
    flip_page();
}

/* view_start @ 2645:527A (linear 0x2B6CA, 17 bytes) */

void view_start(void)
{
    ap7d7cv = g_oldF = 0;
    select_draw_page();
    road_draw_frame();
}
