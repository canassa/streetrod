#include "STRUCTS.H"
#include "M_123F4.H"
#include "M_00000.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_15396.H"
#include "M_163BE.H"
#include "M_193D4.H"
#include "M_19ED0.H"
#include "HOTSPOT.H"
#include "MESSAGES.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_06476.H"
#include "M_113A2.H"
#include "M_1923C.H"
#include <string.h>

static int near F_1287B(void);
static int near F_1289C(int *px, int *py);
static void near F_12CD1(void);
static void near F_133CB(struct Link *arr, struct Link *item, int num);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
void (far *bl78ce)(int, int, int, int, int, struct Bitmap far *);

/* F_123F4: draw the 64 x 6 bar for slot of the dialog; frame selects the highlighted frame.
   When frame == 1 and the screen is shadowed, the old bar is grabbed and pasted first. */

char g_06fc = 1;   /* DS:06FC */
int g_06fe[2] = {78, 93};   /* DS:06FE */
int g_0702[3][2] = {{24, 68}, {24, 68}, {16, 68}};   /* DS:0702 */
int g_070e[6] = {109, 112, 120, 111, 124, 116};   /* DS:070E */
int g_071a[12] = {75, 113, 64, 122, 83, 110, 72, 119, 83, 118, 72, 127};   /* DS:071A */
int g_0732[8] = {68, 74, 144, 74, 68, 74, 160, 74};   /* DS:0732 */
int g_0742[10][2] = {{90, 70}, {98, 70}, {94, 70}, {64, 70}, {90, 70}, {115, 70}, {72, 71}, {111, 71}, {71, 69}, {115, 69}};   /* DS:0742 */
int g_076a[10] = {10, -1, 21, -1, 11, -1, 29, -1, 11, -1};   /* DS:076A */
char g_077e[14] = {29, 0, -1, -1, 2, 2, 2, 2, 3, 4, 2, 3, 2, 3};   /* DS:077E */
char g_078c[6] = {5, 6, 7, 10, 12, 0};   /* DS:078C */
struct Bitmap far *g_0792 = 0;   /* DS:0792 */
struct NewLink g_0796 = {11, 0, 0, 1, 0, 0, 0, 0, ""};   /* DS:0796 */

void F_123F4(int frame, int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    x = 0xb8;
    y = g_06fe[slot] + 10;
    if (frame == 1 && r_vid.f16) {
        blt.w = 0x10;
        blt.h = 6;
        blt.sx = 0x40;
        blt.sy = 0;
        tmp = F_1A89E(pict, &blt, 2);
        (*bl78ce)(0xe, 0x94, x, 8, y, tmp);
        F_19438(1);
    }
    blt.sx = frame << 4;
    blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*b_78a2)(pict, r_vid.back, &blt, 1);
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
    F_19438(1);
}

/* F_12525: draw the bar for slot "slot" of the dialog, then (screen shadowed) grab it back and paste the frame */

void F_12525(int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    blt.sx = blt.sy = 0;
    blt.dx = x = 0xb8;
    blt.dy = y = g_06fe[slot] + 10;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*b_78a2)(pict, r_vid.back, &blt, 1);
    F_120FE(-1);
    if (r_vid.f16) {
        F_1A92A(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
        F_16CF3();
        F_120FE(-4);
        blt.sx = 0x40;
        tmp = F_1A89E(pict, &blt, 2);
        (*bl78ce)(0xe, 0x94, x, y, 0x94, tmp);
        F_19438(1);
        F_120FE(-2);
    }
    F_19438(1);
}

/* F_1264E: show picture n centered in the dialog area; flag moves it left by 24.
   With the screen shadowed the area under it is saved around the draw and restored afterwards. */

void F_1264E(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    struct Blt blt;
    int w1;
    int h1;
    int sx;
    int sy;

    F_120FE(-4);
    pict = F_15BBE(n, 2);
    sx = 200 - (w1 = pict->width);
    sy = 125 - (h1 = pict->height);
    if (flag)
        sx -= 0x18;
    if (r_vid.f16) {
        saved = F_11A72(sx - 8, sy, w1 + 8, h1, 0);
        (*bl78ce)(0xe, 0x94, sx - 8, 0x94, sy, pict);
        blt.sx = blt.sy = 0;
        blt.dx = 8;
        blt.dy = 0;
        blt.w = w1 + 8;
        blt.h = h1;
        (*ab78aat)(pict, saved, &blt, 1);
        F_120FE(-2);
        F_11BD3(saved, sx - 8, sy);
        F_19438(1);
    } else {
        (*bl78ce)(0xe, 0x94, sx, sy, sy, pict);
        F_120FE(-2);
    }
    F_19438(1);
}

/* F_12784: paste picture 0x42e at (0x20, 10) */

void F_12784(int n, int flag)
{
    struct Bitmap far *p;

    F_120FE(-3);
    p = F_15BBE(0x42e, 2);
    F_120FE(-1);
    F_11BD3(p, 0x20, 10);
    F_19438(1);
}

/* F_127DB: redraw the dialog picture n and its two bars; s gives the bar states.
   The screen shadow flag is switched off while drawing and the area is copied to the front afterwards. */

void F_127DB(int n, char far *s)
{
    int saved;
    int i;

    saved = r_vid.f16;
    if (n) {
        g_06fc = 0;
        r_vid.f16 = 0;
        F_120FE(-3);
        F_1264E(n, 0);
        for (i = 0; i < 2; i++)
            F_123F4(*s++, i);
        F_120FE(-1);
        if (r_vid.f16 = saved)
            F_1A92A(r_vid.back, r_vid.front, 0x20, 10, 0xe7, 0xb8);
        g_06fc = 1;
    }
}

struct Blt u_70ac;
int (far *am78a6t)(struct Bitmap far *, struct Bitmap far *, struct Blt *);
int o_8290[5];
int af829ax[1];
int ad829cy[8];
int ap82acz;
int hz82ae;
int g82b2t;

/* F_1287B: slot number for the item in o_8290: 0x434..0x436 map to 0..2 (anything else is returned as it is) */

static int near F_1287B(void)
{
    switch (o_8290[0]) {
    case 0x434:
        return 0;
    case 0x435:
        return 1;
    case 0x436:
        return 2;
    }
}

/* F_1289C: position (x, y) of the current slot in the lower table; returns the offset for the item type */

static int near F_1289C(int *px, int *py)
{
    int r;
    int t;

    switch (o_8290[g_077e[hz82ae]]) {
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
    *px = g_0742[t = hz82ae - 4][0];
    *py = g_0742[t][1];
    return r;
}

/* F_12909: screen position (*px, *py) of the current slot, by game mode hz82ae */

void F_12909(int *px, int *py)
{
    int n;

    switch (hz82ae) {
    default:
        F_1289C(px, py);
        break;
    case 0:
        *px = g_0702[n = F_1287B()][0];
        *py = g_0702[n][1];
        break;
    case 1:
        *px = 0x38;
        *py = 0x4c;
        break;
    }
}

/* F_12964: position (*px, *py) for the current item: looked up per slot, with an offset added in one mode */

void F_12964(int *px, int *py)
{
    int slot;
    int item;

    switch (hz82ae) {
    default:
        slot = F_1289C(px, py);
        *px += g_076a[(slot += g82b2t) * 2];
        *py += g_076a[slot * 2 + 1];
        break;
    case 0:
        *px = g_070e[(slot = F_1287B()) * 2];
        *py = g_070e[slot * 2 + 1];
        break;
    case 1:
        item = o_8290[1];
        slot = (item == 0x438 || item == 0x437) ? 0 : 2;
        if (g82b2t == 1)
            slot++;
        *px = g_0732[slot * 2];
        *py = g_0732[slot * 2 + 1];
        break;
    }
}

/* F_12A17: draw picture n (item id) at the current slot position and restore the background strip
   behind it. With the screen shadowed the picture is first copied into a bitmap 8 pixels wider. */

void F_12A17(int n)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused;
    struct Blt move;
    int px;
    int height;
    int flag;
    int ypos;

    flag = 0;
    F_12964(&px, &ypos);
    F_120FE(-4);
    saved = pict = F_15BBE(n, 2);
    ypos -= saved->height;
    height = move.h = move.fc = saved->height;
    px -= 2;
    move.sy = ypos;
    move.dy = ypos += 9;
    move.dx = (move.sx = px) + 0x60;
    move.w = 6;
    (*am78a6t)(r_vid.f8, r_vid.back, &move);
    if (flag = px & ~r_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, flag, saved);
    }
    px = (px & r_vid.mask) + 0x60;
    (*bl78ce)(0xd, 0x93, px, (n == 0x43f && r_vid.f16) ? 0xd - height : ypos, ypos, saved);
    F_120FE(-2);
    F_19438(1);
    if (pict != saved)
        F_19438(1);
}

/* F_12B7C: redraw the marker picture (0x43f) at the current slot position, aligned to the pixel grid.
   With the screen shadowed the area is also copied to the front buffer and a label is drawn beside it. */

void F_12B7C(void)
{
    int ypos;
    int rows;
    int xpos;
    int y1;
    int wide;
    int x1;
    struct Blt blt;
    struct Bitmap far *pict;

    F_12964(&xpos, &ypos);
    x1 = xpos - 2;
    y1 = ypos;
    F_120FE(-4);
    pict = F_15BBE(0x43f, 2);
    wide = 8;
    if ((rows = xpos & 7) < 2) {
        xpos -= 8;
        wide = 0x10;
    } else if (rows > 4)
        wide = 0x10;
    xpos &= r_vid.mask;
    blt.w = pict->width;
    rows = blt.h = blt.fc = pict->height;
    blt.sx = x1;
    blt.dx = x1 += 0x60;
    y1 -= rows;
    blt.sy = y1;
    blt.dy = y1 += 9;
    (*am78a6t)(r_vid.f8, r_vid.back, &blt);
    if (r_vid.f16) {
        xpos += 0x60;
        ypos += 9 - rows;
        F_1A92A(r_vid.back, r_vid.front, xpos, ypos, xpos + wide, ypos + rows);
        F_16CF3();
        if (wide == 0x10)
            xpos += 8;
        (*bl78ce)(0xd, 0x93, xpos - 8, ypos + 3, 0x93, pict);
    }
    F_120FE(-2);
    F_19438(1);
}

/* F_12CD1: for the marker items 0x43f/0x440 of the current slot, save a copy of the picture
   (shifted to the pixel grid) in g_0792 and set up the blit u_70ac that draws it. */

static void near F_12CD1(void)
{
    int what;
    char kind;
    int px;
    int i;
    int py;
    struct Bitmap far *picture;

    if (o_8290[1] == 0x437) {
        kind = g_077e[hz82ae];
        if (kind == 3)
            return;
        what = ad829cy[i = (kind != 2) ? 1 : 0];
        if (what != 0x43f && what != 0x440)
            return;
        px = g_0732[i * 2] - 2;
        py = g_0732[i * 2 + 1];
        picture = F_15BBE(what, 1);
        g_0792 = F_1934E(picture->width + 8, picture->height, picture->alt.p != 0, 2);
        F_1A03F(picture, px & ~r_vid.mask, g_0792);
        F_1941E(1);
        u_70ac.w = g_0792->width;
        u_70ac.h = g_0792->height;
        u_70ac.sx = u_70ac.sy = 0;
        u_70ac.dx = (px & r_vid.mask) + 0x60;
        u_70ac.dy = py - u_70ac.h + 9;
    }
}

/* F_12DD8: draw item picture n at the current slot position, aligned to the pixel grid;
   flag (nonzero) raises it by 20 and skips the blit to the back buffer. Frees the temporary copies. */

void F_12DD8(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    long unused;
    int w;
    struct Blt move;
    int x;
    int height;
    int z;
    int ypos;
    int mask;

    z = 0;
    F_12909(&x, &ypos);
    if (g_06fc)
        F_120FE(-4);
    saved = pict = F_15BBE(n, 2);
    w = saved->width;
    if (mask = x & ~r_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, mask, saved);
        w = saved->width;
    }
    x &= r_vid.mask;
    if (n == 0x43c)
        F_12CD1();
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
        (*ab78aat)(saved, r_vid.f8, &move, 1);
    }
    if (g_06fc) {
        (*bl78ce)(0xd, 0x93, x + 0x60, r_vid.f16 ? 0xd - height : ypos, ypos + 9, saved);
        F_120FE(-2);
    }
    if (g_0792) {
        F_19438(1);
        g_0792 = 0;
    }
    F_19438(1);
    if (pict != saved)
        F_19438(1);
}

/* F_12F99: draw item picture n at the current slot position, 20 above it, via the r_vid.f8 buffer,
   and copy the area to the front buffer when the screen is shadowed. */

void F_12F99(int n)
{
    struct Bitmap far *pict;
    long unused;
    int w;
    struct Blt move;
    int h;
    int px;
    int py;

    F_12909(&px, &py);
    py -= 0x14;
    F_120FE(-3);
    pict = F_15BBE(n, 2);
    if (n != 0x434 && n != 0x435 && n != 0x436)
        py -= pict->height;
    move.sx = px;
    move.dx = px += 0x60;
    move.sy = py;
    move.dy = py += 9;
    w = move.w = pict->width + 8;
    h = move.h = move.fc = pict->height;
    (*b_78a2)(r_vid.f8, r_vid.back, &move, 1);
    F_19438(1);
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, px, py, px + w, py + h);
}

/* F_1309C: draw the current pair of slot pictures: the strip from r_vid.f8 and the sprite ap82acz,
   copying the area to the front buffer when the screen is shadowed. */

void F_1309C(void)
{
    int next;
    int ys;
    int pair;
    int y2;
    int xs;
    int x2;
    struct Blt blt;

    pair = F_1287B() * 2;
    next = (ap82acz == 0x446) ? pair++ : pair + 1;
    F_120FE(-3);
    blt.w = 0x20;
    xs = blt.dx = (blt.sx = g_071a[next * 2]) + 0x60;
    ys = blt.dy = (blt.sy = g_071a[next * 2 + 1]) + 9;
    blt.h = r_vid.f8->height - blt.sy - 4;
    (*b_78a2)(r_vid.f8, r_vid.back, &blt, 1);
    F_1A734(r_vid.back, ap82acz, (unsigned char)(x2 = g_071a[pair * 2] + 0x60), (unsigned char)(y2 = g_071a[pair * 2 + 1] + 9), 0);
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x2 > xs ? xs : x2, y2 > ys ? ys : y2, blt.w + x2 + 0x18, blt.h + y2 + 0x18);
}

/* F_131B5: draw the item pictures of a car's parts for slots 0, 1 and then 2..4, with the
   screen shadow off (g_06fc). Loads the r_vid.f8 strip first when it isn't there yet. */

void F_131B5(struct Car *car)
{
    int mode;
    int slot;
    int type;
    int other;

    mode = hz82ae;
    g_06fc = 0;
    if (r_vid.f8 == 0)
        r_vid.f8 = F_15BBE(0x42d, 1);
    else
        F_1592E(0x42d, r_vid.f8);
    if (type = o_8290[0]) {
        hz82ae = 0;
        F_12DD8(type, 0);
        if (type == 0x434)
            g_078c[0] = 4;
        else
            g_078c[0] = 5;
        if (type = o_8290[1]) {
            hz82ae = 1;
            F_12DD8(type, 0);
            slot = g_078c[((struct Row8 *)((char *)g_4806 + 2))[car->rack.s[1]->idx].kind];
            for (type = 0; type < 3; type++) {
                hz82ae = slot++;
                if (other = o_8290[type + 2])
                    F_12DD8(other, 0);
            }
        }
    }
    g_06fc = 1;
    hz82ae = mode;
}

/* F_132AD: redraw all nine slot pictures (a and b give each slot's mode and sub-mode) into the
   back buffer over the r_vid.f8 strip, then the ap82acz sprite, and an optional label when flag is set.
   The screen shadow r_vid.f16 is switched off meanwhile and the area is copied to the front afterwards. */

void F_132AD(int *a, int *b, int flag)
{
    int prev2;
    int prev;
    int id;
    int i;
    int shadow;
    struct Blt blt;

    prev = hz82ae;
    prev2 = g82b2t;
    shadow = r_vid.f16;
    r_vid.f16 = 0;
    blt.sx = 0;
    blt.dx = 0x60;
    blt.sy = 0;
    blt.dy = 9;
    blt.w = r_vid.f8->width;
    blt.h = blt.fc = r_vid.f8->height;
    (*b_78a2)(r_vid.f8, r_vid.back, &blt, 1);
    for (i = 0; i < 9; i++) {
        if (id = af829ax[i]) {
            hz82ae = a[i];
            g82b2t = b[i];
            F_12A17(id);
        }
    }
    if (ap82acz)
        F_1309C();
    if (flag) {
        F_119B8(0x4c6, 0x60, 0x94, -1);
        blt.h = 0xaf;
    }
    if (r_vid.f16 = shadow)
        F_1A92A(r_vid.back, r_vid.front, blt.dx, blt.dy, blt.dx + blt.w - 1, blt.dy + blt.h - 1);
    hz82ae = prev;
    g82b2t = prev2;
}

/* F_133CB: insert item into the array arr of n records, which is sorted by (f8, fa): it goes before
   the first record that compares greater, the rest are moved up by one. */

static void near F_133CB(struct Link *arr, struct Link *item, int n)
{
    struct Link *p;
    int i;

    p = arr;
    for (i = 0; i < n; i++, p++) {
        if (item->f8 < p->f8) {
            memmove(p + 1, p, (n - i) * sizeof(struct Link));
            *p = *item;
            return;
        }
        if (item->f8 <= p->f8 && item->fa < p->fa) {
            memmove(p + 1, p, (n - i) * sizeof(struct Link));
            *p = *item;
            return;
        }
    }
    *p = *item;
}

/* F_1344A: build the sorted list of menu items in g_184c: the 3 fixed records are copied from g_1948,
   then for each of 9 slots (grouped by the counts in g_5130) that is enabled in o_8290 an item is
   inserted at the position F_12964 gives for the slot's coordinates, plus a last item picked by
   ap82acz. hz82ae/g82b2t are saved and restored around it. */

#pragma intrinsic(memcpy)

void F_1344A(int *xs, int *ys)
{
    int lane;
    int num;
    int x, i, y;
    int rh, rx;
    int j;
    struct Link *item;
    int len;

    rx = hz82ae;
    rh = g82b2t;
    item = g_1948;
    len = (num = 3) * sizeof(struct Link);
    memcpy(g_184c, item, len);
    g_0796.f4 = g_2ba5;
    i = lane = j = 0;
    for (; i < 9; i++) {
        if (o_8290[lane]) {
            hz82ae = xs[i];
            g82b2t = ys[i];
            F_12964(&x, &y);
            x += 0x60;
            y += 9;
            g_0796.f6 = i + 1;
            g_0796.fc = (g_0796.f8 = x - 2) + 5;
            g_0796.fa = (g_0796.fe = y + 2) - 15;
            F_133CB(g_184c, (struct Link *)&g_0796, num);
            num++;
        }
        if (++j == g_5130[lane]) {
            lane++;
            j = 0;
        }
    }
    if (o_8290[0]) {
        g_0796.f4 = g_2ba7;
        g_0796.f6 = 13;
        i = (ap82acz == 0x446) + F_1287B() * 2;
        g_0796.fc = (g_0796.f8 = g_071a[i * 2] + 0x5b) + 10;
        g_0796.fe = (g_0796.fa = g_071a[i * 2 + 1] + 2) + 15;
        F_133CB(g_184c, (struct Link *)&g_0796, num);
        num++;
    }
    g_184c[num].nxt = 9;
    hz82ae = rx;
    g82b2t = rh;
    F_00D78();
}

/* F_135E5: sort n hot spots of g_09f4 starting at first by x1 (exchange sort, ascending) */

void F_135E5(int first, int n)
{
    struct Hot *p;
    struct Hot *q;
    int i;
    int k;
    struct Hot swap;

    for (i = 0; i < n - 1; i++) {
        p = &g_09f4[first + i];
        for (k = i + 1; k < n; k++) {
            q = &g_09f4[first + k];
            if (p->x1 > q->x1) {
                swap = *p;
                *p = *q;
                *q = swap;
            }
        }
    }
}
