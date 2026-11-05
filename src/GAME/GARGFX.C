#include <string.h>
#include "GARGFX.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "GAME.H"
#include "GARAGE.H"
#include "HOTSPOT.H"
#include "INPUTDAT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MESSAGES.H"
#include "POPUP.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"

static int near engine_index(void);
static int near carb_pos(int *px, int *py);
static void near make_bolt_overlay(void);
static void near insert_hot(struct Hot *arr, struct Hot *item, int num);


static char g_partAnimate = 1;
static int g_tboltY[2] = {78, 93};
static int g_enginePos[3][2] = {{24, 68}, {24, 68}, {16, 68}};
static int g_engineBoltPos[6] = {109, 112, 120, 111, 124, 116};
static int g_wiresPos[12] = {75, 113, 64, 122, 83, 110, 72, 119, 83, 118, 72, 127};
static int g_manifoldBoltPos[8] = {68, 74, 144, 74, 68, 74, 160, 74};
static int g_carbPos[10][2] = {{90, 70},  {98, 70}, {94, 70},  {64, 70}, {90, 70},
                               {115, 70}, {72, 71}, {111, 71}, {71, 69}, {115, 69}};
static int g_carbBoltOff[10] = {10, -1, 21, -1, 11, -1, 29, -1, 11, -1};
static char g_partSlotMap[14] = {29, 0, -1, -1, 2, 2, 2, 2, 3, 4, 2, 3, 2, 3};
static char g_carbPosBase[6] = {5, 6, 7, 10, 12, 0};
struct Bitmap far *g_boltOverlay = 0;
static struct Hot g_boltHot = {HOT_REGION, 0, 0, 1, 0, 0, 0, 0, 0};

/* Draw the 64 x 6 bar for slot of the dialog; frame selects the highlighted frame.
   When frame == 1 and the screen is shadowed, the old bar is grabbed and pasted first. */

void gx_trans_bolt(int frame, int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    cu_ctl(CURSOR_HIDE);
    pict = di_bm(0x44f, ARENA_HIGH_STACK);
    x = 0xb8;
    y = g_tboltY[slot] + 10;
    if (frame == 1 && r_vid.copyToFront) {
        blt.w = 0x10;
        blt.h = 6;
        blt.sx = 0x40;
        blt.sy = 0;
        tmp = bm_grab(pict, &blt, ARENA_HIGH_STACK);
        (*g_videoOps.slideSprite)(0xe, 0x94, x, 8, y, tmp);
        ma_pop_high(1);
    }
    blt.sx = frame << 4;
    blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    blt.w = 0x10;
    blt.h = blt.unused1 = 6;
    (*g_videoOps.copy)(pict, r_vid.back, &blt, 1);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
    ma_pop_high(1);
}

/* Draw the bar for slot "slot" of the dialog, then (screen shadowed) grab it back and paste the
   frame */

void gx_trans_drop(int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    cu_ctl(CURSOR_HIDE);
    pict = di_bm(0x44f, ARENA_HIGH_STACK);
    blt.sx = blt.sy = 0;
    blt.dx = x = 0xb8;
    blt.dy = y = g_tboltY[slot] + 10;
    blt.w = 0x10;
    blt.h = blt.unused1 = 6;
    (*g_videoOps.copy)(pict, r_vid.back, &blt, 1);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront) {
        bm_copy_box(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
        so_bolt_turn();
        cu_ctl(CURSOR_HIDE_NOW);
        blt.sx = 0x40;
        tmp = bm_grab(pict, &blt, ARENA_HIGH_STACK);
        (*g_videoOps.slideSprite)(0xe, 0x94, x, y, 0x94, tmp);
        ma_pop_high(1);
        cu_ctl(CURSOR_SHOW_NOW);
    }
    ma_pop_high(1);
}

/* Show picture n centered in the dialog area; flag moves it left by 24.
   With the screen shadowed the area under it is saved around the draw and restored afterwards. */

void gx_trans_pic(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    struct Blt blt;
    int w1;
    int h1;
    int sx;
    int sy;

    cu_ctl(CURSOR_HIDE_NOW);
    pict = di_bm(n, ARENA_HIGH_STACK);
    sx = 200 - (w1 = pict->width);
    sy = 125 - (h1 = pict->height);
    if (flag)
        sx -= 0x18;
    if (r_vid.copyToFront) {
        saved = pu_grab(sx - 8, sy, w1 + 8, h1, 0);
        (*g_videoOps.slideSprite)(0xe, 0x94, sx - 8, 0x94, sy, pict);
        blt.sx = blt.sy = 0;
        blt.dx = 8;
        blt.dy = 0;
        blt.w = w1 + 8;
        blt.h = h1;
        (*g_videoOps.maskedBlit)(pict, saved, &blt, 1);
        cu_ctl(CURSOR_SHOW_NOW);
        pu_show(saved, sx - 8, sy);
        ma_pop_high(1);
    } else {
        (*g_videoOps.slideSprite)(0xe, 0x94, sx, sy, sy, pict);
        cu_ctl(CURSOR_SHOW_NOW);
    }
    ma_pop_high(1);
}

/* Paste picture 0x42e at (0x20, 10) */

void gx_trans_clear(int n, int flag)
{
    struct Bitmap far *p;

    cu_ctl(CURSOR_HIDE);
    p = di_bm(0x42e, ARENA_HIGH_STACK);
    cu_ctl(CURSOR_SHOW);
    pu_show(p, 0x20, 10);
    ma_pop_high(1);
}

/* Redraw the dialog picture n and its two bars; s gives the bar states.
   The screen shadow flag is switched off while drawing and the area is copied to the front afterwards. */

void gx_trans_draw(int n, char far *s)
{
    int saved;
    int i;

    saved = r_vid.copyToFront;
    if (n) {
        g_partAnimate = 0;
        r_vid.copyToFront = 0;
        cu_ctl(CURSOR_HIDE);
        gx_trans_pic(n, 0);
        for (i = 0; i < 2; i++)
            gx_trans_bolt(*s++, i);
        cu_ctl(CURSOR_SHOW);
        if (r_vid.copyToFront = saved)
            bm_copy_box(r_vid.back, r_vid.front, 0x20, 10, 0xe7, 0xb8);
        g_partAnimate = 1;
    }
}

struct Blt g_cpyBoltPatchRecord;
int g_partsPics[5];
int g_engineBoltPicIds[9];
struct PartSelection g_slotSelec;

/* Slot number for the item in g_partsPics: 0x434..0x436 map to 0..2 (anything else is returned as it is) */

static int near engine_index(void)
{
    switch (g_partsPics[0]) {
    case 0x434:
        return 0;
    case 0x435:
        return 1;
    case 0x436:
        return 2;
    }
}

/* Position (x, y) of the current slot in the lower table; returns the offset for the item type */

static int near carb_pos(int *px, int *py)
{
    int r;
    int t;

    switch (g_partsPics[g_partSlotMap[g_slotSelec.slot]]) {
    case 0x43c:
        r = 0;
        break;
    case 0x43d:
        r = 2;
        break;
    case 0x43e:
        r = 4;
        break;
    }
    *px = g_carbPos[t = g_slotSelec.slot - 4][0];
    *py = g_carbPos[t][1];
    return r;
}

/* Screen position (*px, *py) of the current slot, by game mode g_slotSelec.slot */

void gx_part_pos(int *px, int *py)
{
    int n;

    switch (g_slotSelec.slot) {
    default:
        carb_pos(px, py);
        break;
    case 0:
        *px = g_enginePos[n = engine_index()][0];
        *py = g_enginePos[n][1];
        break;
    case 1:
        *px = 0x38;
        *py = 0x4c;
        break;
    }
}

/* Position (*px, *py) for the current item: looked up per slot, with an offset added in one mode */

void gx_bolt_pos(int *px, int *py)
{
    int slot;
    int item;

    switch (g_slotSelec.slot) {
    default:
        slot = carb_pos(px, py);
        *px += g_carbBoltOff[(slot += g_slotSelec.subslot) * 2];
        *py += g_carbBoltOff[slot * 2 + 1];
        break;
    case 0:
        *px = g_engineBoltPos[(slot = engine_index()) * 2];
        *py = g_engineBoltPos[slot * 2 + 1];
        break;
    case 1:
        item = g_partsPics[1];
        slot = (item == 0x438 || item == 0x437) ? 0 : 2;
        if (g_slotSelec.subslot == 1)
            slot++;
        *px = g_manifoldBoltPos[slot * 2];
        *py = g_manifoldBoltPos[slot * 2 + 1];
        break;
    }
}

/* Draw picture n (item id) at the current slot position and restore the background strip
   behind it. With the screen shadowed the picture is first copied into a bitmap 8 pixels wider. */

void gx_draw_bolt(int n)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused; /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
    struct Blt move;
    int px;
    int height;
    int flag;
    int ypos;

    flag = 0;
    gx_bolt_pos(&px, &ypos);
    cu_ctl(CURSOR_HIDE_NOW);
    saved = pict = di_bm(n, ARENA_HIGH_STACK);
    ypos -= saved->height;
    height = move.h = move.unused1 = saved->height;
    px -= 2;
    move.sy = ypos;
    move.dy = ypos += 9;
    move.dx = (move.sx = px) + 0x60;
    move.w = 6;
    (*g_videoOps.shiftBlit)(r_vid.hoodBg, r_vid.back, &move);
    if (flag = px & ~r_vid.mask) {
        saved = ar_bm(pict->width + 8, pict->height, pict->alt.p != 0, ARENA_HIGH_STACK);
        bm_shift_right(pict, flag, saved);
    }
    px = (px & r_vid.mask) + 0x60;
    (*g_videoOps.slideSprite)(0xd, 0x93, px, (n == 0x43f && r_vid.copyToFront) ? 0xd - height : ypos, ypos, saved);
    cu_ctl(CURSOR_SHOW_NOW);
    ma_pop_high(1);
    if (pict != saved)
        ma_pop_high(1);
}

/* Redraw the marker picture (0x43f) at the current slot position, aligned to the pixel grid.
   With the screen shadowed the area is also copied to the front buffer and a label is drawn beside it. */

void gx_drop_bolt(void)
{
    int ypos;
    int rows;
    int xpos;
    int y1;
    int wide;
    int x1;
    struct Blt blt;
    struct Bitmap far *pict;

    gx_bolt_pos(&xpos, &ypos);
    x1 = xpos - 2;
    y1 = ypos;
    cu_ctl(CURSOR_HIDE_NOW);
    pict = di_bm(0x43f, ARENA_HIGH_STACK);
    wide = 8;
    if ((rows = xpos & 7) < 2) {
        xpos -= 8;
        wide = 0x10;
    } else if (rows > 4)
        wide = 0x10;
    xpos &= r_vid.mask;
    blt.w = pict->width;
    rows = blt.h = blt.unused1 = pict->height;
    blt.sx = x1;
    blt.dx = x1 += 0x60;
    y1 -= rows;
    blt.sy = y1;
    blt.dy = y1 += 9;
    (*g_videoOps.shiftBlit)(r_vid.hoodBg, r_vid.back, &blt);
    if (r_vid.copyToFront) {
        xpos += 0x60;
        ypos += 9 - rows;
        bm_copy_box(r_vid.back, r_vid.front, xpos, ypos, xpos + wide, ypos + rows);
        so_bolt_turn();
        if (wide == 0x10)
            xpos += 8;
        (*g_videoOps.slideSprite)(0xd, 0x93, xpos - 8, ypos + 3, 0x93, pict);
    }
    cu_ctl(CURSOR_SHOW_NOW);
    ma_pop_high(1);
}

/* For the marker items 0x43f/0x440 of the current slot, save a copy of the picture
   (shifted to the pixel grid) in g_boltOverlay and set up the blit g_cpyBoltPatchRecord that draws it. */

static void near make_bolt_overlay(void)
{
    int what;
    char kind;
    int px;
    int i;
    int py;
    struct Bitmap far *picture;

    if (g_partsPics[1] == 0x437) {
        kind = g_partSlotMap[g_slotSelec.slot];
        if (kind == 3)
            return;
        what = g_engineBoltPicIds[(i = (kind != 2) ? 1 : 0) + 1];
        if (what != 0x43f && what != 0x440)
            return;
        px = g_manifoldBoltPos[i * 2] - 2;
        py = g_manifoldBoltPos[i * 2 + 1];
        picture = di_bm(what, ARENA_LOW_STACK);
        g_boltOverlay = ar_bm(picture->width + 8, picture->height, picture->alt.p != 0, ARENA_HIGH_STACK);
        bm_shift_right(picture, px & ~r_vid.mask, g_boltOverlay);
        ma_pop_low(1);
        g_cpyBoltPatchRecord.w = g_boltOverlay->width;
        g_cpyBoltPatchRecord.h = g_boltOverlay->height;
        g_cpyBoltPatchRecord.sx = g_cpyBoltPatchRecord.sy = 0;
        g_cpyBoltPatchRecord.dx = (px & r_vid.mask) + 0x60;
        g_cpyBoltPatchRecord.dy = py - g_cpyBoltPatchRecord.h + 9;
    }
}

/* Draw item picture n at the current slot position, aligned to the pixel grid;
   flag (nonzero) raises it by 20 and skips the blit to the back buffer. Frees the temporary copies. */

void gx_draw_part(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused; /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
    int w;
    struct Blt move;
    int x;
    int height;
    int z;
    int ypos;
    int mask;

    z = 0;
    gx_part_pos(&x, &ypos);
    if (g_partAnimate)
        cu_ctl(CURSOR_HIDE_NOW);
    saved = pict = di_bm(n, ARENA_HIGH_STACK);
    w = saved->width;
    if (mask = x & ~r_vid.mask) {
        saved = ar_bm(pict->width + 8, pict->height, pict->alt.p != 0, ARENA_HIGH_STACK);
        bm_shift_right(pict, mask, saved);
        w = saved->width;
    }
    x &= r_vid.mask;
    if (n == 0x43c)
        make_bolt_overlay();
    if (n != 0x434 && n != 0x435 && n != 0x436)
        ypos -= saved->height;
    if (flag)
        ypos -= 0x14;
    move.w = w;
    height = move.h = saved->height;
    move.sx = move.sy = 0;
    if (flag == 0) {
        move.dx = x;
        move.dy = ypos;
        (*g_videoOps.maskedBlit)(saved, r_vid.hoodBg, &move, 1);
    }
    if (g_partAnimate) {
        (*g_videoOps.slideSprite)(0xd, 0x93, x + 0x60, r_vid.copyToFront ? 0xd - height : ypos, ypos + 9, saved);
        cu_ctl(CURSOR_SHOW_NOW);
    }
    if (g_boltOverlay) {
        ma_pop_high(1);
        g_boltOverlay = 0;
    }
    ma_pop_high(1);
    if (pict != saved)
        ma_pop_high(1);
}

/* Draw item picture n at the current slot position, 20 above it, via the r_vid.hoodBg buffer,
   and copy the area to the front buffer when the screen is shadowed. */

void gx_erase_part(int n, int flag)
{
    struct Bitmap far *pict;
    long unused; /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
    int w;
    struct Blt move;
    int h;
    int px;
    int py;

    gx_part_pos(&px, &py);
    py -= 0x14;
    cu_ctl(CURSOR_HIDE);
    pict = di_bm(n, ARENA_HIGH_STACK);
    if (n != 0x434 && n != 0x435 && n != 0x436)
        py -= pict->height;
    move.sx = px;
    move.dx = px += 0x60;
    move.sy = py;
    move.dy = py += 9;
    w = move.w = pict->width + 8;
    h = move.h = move.unused1 = pict->height;
    (*g_videoOps.copy)(r_vid.hoodBg, r_vid.back, &move, 1);
    ma_pop_high(1);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, px, py, px + w, py + h);
}

/* Draw the current pair of slot pictures: the strip from r_vid.hoodBg and the sprite g_slotSelec.wirePic,
   copying the area to the front buffer when the screen is shadowed. */

void gx_draw_wires(void)
{
    int next;
    int ys;
    int pair;
    int y2;
    int xs;
    int x2;
    struct Blt blt;

    pair = engine_index() * 2;
    next = (g_slotSelec.wirePic == 0x446) ? pair++ : pair + 1;
    cu_ctl(CURSOR_HIDE);
    blt.w = 0x20;
    xs = blt.dx = (blt.sx = g_wiresPos[next * 2]) + 0x60;
    ys = blt.dy = (blt.sy = g_wiresPos[next * 2 + 1]) + 9;
    blt.h = r_vid.hoodBg->height - blt.sy - 4;
    (*g_videoOps.copy)(r_vid.hoodBg, r_vid.back, &blt, 1);
    bm_draw_pic(r_vid.back, g_slotSelec.wirePic, (unsigned char)(x2 = g_wiresPos[pair * 2] + 0x60),
                (unsigned char)(y2 = g_wiresPos[pair * 2 + 1] + 9), 0);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x2 > xs ? xs : x2, y2 > ys ? ys : y2, blt.w + x2 + 0x18,
                    blt.h + y2 + 0x18);
}

/* Draw the item pictures of a car's parts for slots 0, 1 and then 2..4, with the
   screen shadow off (g_partAnimate). Loads the r_vid.hoodBg strip first when it isn't there yet. */

void gx_draw_parts(struct Car *car)
{
    int mode;
    int slot;
    int type;
    int other;

    mode = g_slotSelec.slot;
    g_partAnimate = 0;
    if (r_vid.hoodBg == 0)
        r_vid.hoodBg = di_bm(0x42d, ARENA_LOW_STACK);
    else
        di_unpack_pic(0x42d, r_vid.hoodBg);
    if (type = g_partsPics[0]) {
        g_slotSelec.slot = 0;
        gx_draw_part(type, 0);
        if (type == 0x434)
            g_carbPosBase[0] = 4;
        else
            g_carbPosBase[0] = 5;
        if (type = g_partsPics[1]) {
            g_slotSelec.slot = 1;
            gx_draw_part(type, 0);
            slot = g_carbPosBase[g_parts[car->rack.parts[1]->idx].variant];
            for (type = 0; type < 3; type++) {
                g_slotSelec.slot = slot++;
                if (other = g_partsPics[type + 2])
                    gx_draw_part(other, 0);
            }
        }
    }
    g_partAnimate = 1;
    g_slotSelec.slot = mode;
}

/* Redraw all nine slot pictures (a and b give each slot's mode and sub-mode) into the
   back buffer over the r_vid.hoodBg strip, then the g_slotSelec.wirePic sprite, and an optional label when flag is set.
   The screen shadow r_vid.copyToFront is switched off meanwhile and the area is copied to the front afterwards. */

void gx_draw_bolts(int *a, int *b, int flag)
{
    int prev2;
    int prev;
    int id;
    int i;
    int shadow;
    struct Blt blt;

    prev = g_slotSelec.slot;
    prev2 = g_slotSelec.subslot;
    shadow = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    blt.sx = 0;
    blt.dx = 0x60;
    blt.sy = 0;
    blt.dy = 9;
    blt.w = r_vid.hoodBg->width;
    blt.h = blt.unused1 = r_vid.hoodBg->height;
    (*g_videoOps.copy)(r_vid.hoodBg, r_vid.back, &blt, 1);
    for (i = 0; i < 9; i++) {
        if (id = g_engineBoltPicIds[i]) {
            g_slotSelec.slot = a[i];
            g_slotSelec.subslot = b[i];
            gx_draw_bolt(id);
        }
    }
    if (g_slotSelec.wirePic)
        gx_draw_wires();
    if (flag) {
        sc_picture_at(0x4c6, 0x60, 0x94, -1);
        blt.h = 0xaf;
    }
    if (r_vid.copyToFront = shadow)
        bm_copy_box(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + blt.w - 1, blt.dy + blt.h - 1);
    g_slotSelec.slot = prev;
    g_slotSelec.subslot = prev2;
}

/* Insert item into the array arr of n records, which is sorted by (x1, y1): it goes before
   the first record that compares greater, the rest are moved up by one. */

static void near insert_hot(struct Hot *arr, struct Hot *item, int n)
{
    struct Hot *p;
    int i;

    p = arr;
    for (i = 0; i < n; i++, p++) {
        if (item->x1 < p->x1) {
            memmove(p + 1, p, (n - i) * sizeof(struct Hot));
            *p = *item;
            return;
        }
        if (item->x1 <= p->x1 && item->y1 < p->y1) {
            memmove(p + 1, p, (n - i) * sizeof(struct Hot));
            *p = *item;
            return;
        }
    }
    *p = *item;
}

/* Build the sorted list of menu items in g_engineHots: the 3 fixed records are copied from g_engineHotTemplate,
   then for each of 9 slots (grouped by the counts in g_nBolt) that is enabled in g_partsPics an item is
   inserted at the position gx_bolt_pos gives for the slot's coordinates, plus a last item picked by
   g_slotSelec.wirePic. g_slotSelec.slot/g_slotSelec.subslot are saved and restored around it. */

void gx_build_hots(int *xs, int *ys)
{
    int lane;
    int num;
    int x, i, y;
    int rh, rx;
    int j;
    struct Hot *item;
    int len;

    rx = g_slotSelec.slot;
    rh = g_slotSelec.subslot;
    item = g_engineHotTemplate;
    len = (num = 3) * sizeof(struct Hot);
    memcpy(g_engineHots, item, len);
    g_boltHot.textOffset = g_offBolt;
    i = lane = j = 0;
    for (; i < 9; i++) {
        if (g_partsPics[lane]) {
            g_slotSelec.slot = xs[i];
            g_slotSelec.subslot = ys[i];
            gx_bolt_pos(&x, &y);
            x += 0x60;
            y += 9;
            g_boltHot.id = i + 1;
            g_boltHot.x2 = (g_boltHot.x1 = x - 2) + 5;
            g_boltHot.y1 = (g_boltHot.y2 = y + 2) - 15;
            insert_hot(g_engineHots, &g_boltHot, num);
            num++;
        }
        if (++j == g_nBolt[lane]) {
            lane++;
            j = 0;
        }
    }
    if (g_partsPics[0]) {
        g_boltHot.textOffset = g_offWires;
        g_boltHot.id = 13;
        i = (g_slotSelec.wirePic == 0x446) + engine_index() * 2;
        g_boltHot.x2 = (g_boltHot.x1 = g_wiresPos[i * 2] + 0x5b) + 10;
        g_boltHot.y2 = (g_boltHot.y1 = g_wiresPos[i * 2 + 1] + 2) + 15;
        insert_hot(g_engineHots, &g_boltHot, num);
        num++;
    }
    g_engineHots[num].kind = HOT_END;
    g_slotSelec.slot = rx;
    g_slotSelec.subslot = rh;
    sy_hover_reset();
}

/* Sort n hot spots of g_hots starting at first by x1 (exchange sort, ascending) */

void gx_sort_hots(int first, int n)
{
    struct Hot *p;
    struct Hot *q;
    int i;
    int k;
    struct Hot swap;

    for (i = 0; i < n - 1; i++) {
        p = &g_hots[first + i];
        for (k = i + 1; k < n; k++) {
            q = &g_hots[first + k];
            if (p->x1 > q->x1) {
                swap = *p;
                *p = *q;
                *q = swap;
            }
        }
    }
}
