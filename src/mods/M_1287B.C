#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>

static int near F_1287B(void);
static int near F_1289C(int *px, int *py);
static void near F_12CD1(void);
static void near F_133CB(struct Link *arr, struct Link *item, int num);

/* F_1287B: slot number for the item in g_8290: 0x434..0x436 map to 0..2 (anything else is returned as it is) */


static int near F_1287B(void)
{
    switch (g_8290[0]) {
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

    switch (g_8290[g_077e[g_82ae]]) {
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
    *px = g_0742[t = g_82ae - 4][0];
    *py = g_0742[t][1];
    return r;
}

/* F_12909: screen position (*px, *py) of the current slot, by game mode g_82ae */



void F_12909(int *px, int *py)
{
    int n;

    switch (g_82ae) {
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

    switch (g_82ae) {
    default:
        slot = F_1289C(px, py);
        *px += g_076a[(slot += g_82b2) * 2];
        *py += g_076a[slot * 2 + 1];
        break;
    case 0:
        *px = g_070e[(slot = F_1287B()) * 2];
        *py = g_070e[slot * 2 + 1];
        break;
    case 1:
        item = g_8290[1];
        slot = (item == 0x438 || item == 0x437) ? 0 : 2;
        if (g_82b2 == 1)
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
    (*g_78a6)(g_vid.f8, g_vid.back, &move);
    if (flag = px & ~g_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, flag, saved);
    }
    px = (px & g_vid.mask) + 0x60;
    (*g_78ce)(0xd, 0x93, px, (n == 0x43f && g_vid.f16) ? 0xd - height : ypos, ypos, saved);
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
    xpos &= g_vid.mask;
    blt.w = pict->width;
    rows = blt.h = blt.fc = pict->height;
    blt.sx = x1;
    blt.dx = x1 += 0x60;
    y1 -= rows;
    blt.sy = y1;
    blt.dy = y1 += 9;
    (*g_78a6)(g_vid.f8, g_vid.back, &blt);
    if (g_vid.f16) {
        xpos += 0x60;
        ypos += 9 - rows;
        F_1A92A(g_vid.back, g_vid.front, xpos, ypos, xpos + wide, ypos + rows);
        F_16CF3();
        if (wide == 0x10)
            xpos += 8;
        (*g_78ce)(0xd, 0x93, xpos - 8, ypos + 3, 0x93, pict);
    }
    F_120FE(-2);
    F_19438(1);
}

/* F_12CD1: for the marker items 0x43f/0x440 of the current slot, save a copy of the picture
   (shifted to the pixel grid) in g_0792 and set up the blit g_70ac that draws it. */


static void near F_12CD1(void)
{
    int what;
    char kind;
    int px;
    int i;
    int py;
    struct Bitmap far *picture;

    if (g_8290[1] == 0x437) {
        kind = g_077e[g_82ae];
        if (kind == 3)
            return;
        what = g_829c[i = (kind != 2) ? 1 : 0];
        if (what != 0x43f && what != 0x440)
            return;
        px = g_0732[i * 2] - 2;
        py = g_0732[i * 2 + 1];
        picture = F_15BBE(what, 1);
        g_0792 = F_1934E(picture->width + 8, picture->height, picture->alt.p != 0, 2);
        F_1A03F(picture, px & ~g_vid.mask, g_0792);
        F_1941E(1);
        g_70ac.w = g_0792->width;
        g_70ac.h = g_0792->height;
        g_70ac.sx = g_70ac.sy = 0;
        g_70ac.dx = (px & g_vid.mask) + 0x60;
        g_70ac.dy = py - g_70ac.h + 9;
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
    if (mask = x & ~g_vid.mask) {
        saved = F_1934E(pict->width + 8, pict->height, pict->alt.p != 0, 2);
        F_1A03F(pict, mask, saved);
        w = saved->width;
    }
    x &= g_vid.mask;
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
        (*g_78aa)(saved, g_vid.f8, &move, 1);
    }
    if (g_06fc) {
        (*g_78ce)(0xd, 0x93, x + 0x60, g_vid.f16 ? 0xd - height : ypos, ypos + 9, saved);
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

/* F_12F99: draw item picture n at the current slot position, 20 above it, via the g_vid.f8 buffer,
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
    (*g_78a2)(g_vid.f8, g_vid.back, &move, 1);
    F_19438(1);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, px, py, px + w, py + h);
}

/* F_1309C: draw the current pair of slot pictures: the strip from g_vid.f8 and the sprite g_82ac,
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
    next = (g_82ac == 0x446) ? pair++ : pair + 1;
    F_120FE(-3);
    blt.w = 0x20;
    xs = blt.dx = (blt.sx = g_071a[next * 2]) + 0x60;
    ys = blt.dy = (blt.sy = g_071a[next * 2 + 1]) + 9;
    blt.h = g_vid.f8->height - blt.sy - 4;
    (*g_78a2)(g_vid.f8, g_vid.back, &blt, 1);
    F_1A734(g_vid.back, g_82ac, (unsigned char)(x2 = g_071a[pair * 2] + 0x60), (unsigned char)(y2 = g_071a[pair * 2 + 1] + 9), 0);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, x2 > xs ? xs : x2, y2 > ys ? ys : y2, blt.w + x2 + 0x18, blt.h + y2 + 0x18);
}

/* F_131B5: draw the item pictures of a car's parts for slots 0, 1 and then 2..4, with the
   screen shadow off (g_06fc). Loads the g_vid.f8 strip first when it isn't there yet. */


void F_131B5(struct Car *car)
{
    int mode;
    int slot;
    int type;
    int other;

    mode = g_82ae;
    g_06fc = 0;
    if (g_vid.f8 == 0)
        g_vid.f8 = F_15BBE(0x42d, 1);
    else
        F_1592E(0x42d, g_vid.f8);
    if (type = g_8290[0]) {
        g_82ae = 0;
        F_12DD8(type, 0);
        if (type == 0x434)
            g_078c[0] = 4;
        else
            g_078c[0] = 5;
        if (type = g_8290[1]) {
            g_82ae = 1;
            F_12DD8(type, 0);
            slot = g_078c[g_4808[car->rack.s[1]->idx].kind];
            for (type = 0; type < 3; type++) {
                g_82ae = slot++;
                if (other = g_8290[type + 2])
                    F_12DD8(other, 0);
            }
        }
    }
    g_06fc = 1;
    g_82ae = mode;
}

/* F_132AD: redraw all nine slot pictures (a and b give each slot's mode and sub-mode) into the
   back buffer over the g_vid.f8 strip, then the g_82ac sprite, and an optional label when flag is set.
   The screen shadow g_vid.f16 is switched off meanwhile and the area is copied to the front afterwards. */


void F_132AD(int *a, int *b, int flag)
{
    int prev2;
    int prev;
    int id;
    int i;
    int shadow;
    struct Blt blt;

    prev = g_82ae;
    prev2 = g_82b2;
    shadow = g_vid.f16;
    g_vid.f16 = 0;
    blt.sx = 0;
    blt.dx = 0x60;
    blt.sy = 0;
    blt.dy = 9;
    blt.w = g_vid.f8->width;
    blt.h = blt.fc = g_vid.f8->height;
    (*g_78a2)(g_vid.f8, g_vid.back, &blt, 1);
    for (i = 0; i < 9; i++) {
        if (id = g_829a[i]) {
            g_82ae = a[i];
            g_82b2 = b[i];
            F_12A17(id);
        }
    }
    if (g_82ac)
        F_1309C();
    if (flag) {
        F_119B8(0x4c6, 0x60, 0x94, -1);
        blt.h = 0xaf;
    }
    if (g_vid.f16 = shadow)
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + blt.w - 1, blt.dy + blt.h - 1);
    g_82ae = prev;
    g_82b2 = prev2;
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
   then for each of 9 slots (grouped by the counts in g_5130) that is enabled in g_8290 an item is
   inserted at the position F_12964 gives for the slot's coordinates, plus a last item picked by
   g_82ac. g_82ae/g_82b2 are saved and restored around it. */

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

    rx = g_82ae;
    rh = g_82b2;
    item = g_1948;
    len = (num = 3) * sizeof(struct Link);
    memcpy(g_184c, item, len);
    g_0796.f4 = g_2ba5;
    i = lane = j = 0;
    for (; i < 9; i++) {
        if (g_8290[lane]) {
            g_82ae = xs[i];
            g_82b2 = ys[i];
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
    if (g_8290[0]) {
        g_0796.f4 = g_2ba7;
        g_0796.f6 = 13;
        i = (g_82ac == 0x446) + F_1287B() * 2;
        g_0796.fc = (g_0796.f8 = g_071a[i * 2] + 0x5b) + 10;
        g_0796.fe = (g_0796.fa = g_071a[i * 2 + 1] + 2) + 15;
        F_133CB(g_184c, (struct Link *)&g_0796, num);
        num++;
    }
    g_184c[num].nxt = 9;
    g_82ae = rx;
    g_82b2 = rh;
    F_00D78();
}
