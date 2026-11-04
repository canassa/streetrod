#include <stdlib.h>
#include "STRUCTS.H"
#include "M_1947A.H"
#include "BITMAP.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_10B7C.H"
#include "M_11A72.H"
#include "M_123F4.H"
#include "M_174A6.H"
#include "M_1923C.H"
#include "M_193D4.H"

#pragma intrinsic(abs)

/* F_1947A @ 0F38:A0FA (linear 0x1947A, 158 bytes)

   Intersect rectangles a and b into out, snapped to the r_vid.mask pixel mask; 0 if they do not
   overlap */

struct Rect *F_1947A(struct Rect *a, struct Rect *b, struct Rect *out)
{
    if (b->x2 < a->x1 || a->x2 < b->x1)
        return 0;
    out->x1 = (a->x1 < b->x1) ? b->x1 : a->x1;
    out->x2 = (a->x2 > b->x2) ? b->x2 : a->x2;
    if (a->y1 > b->y2 || a->y2 < b->y1)
        return 0;
    out->y1 = (a->y1 < b->y1) ? b->y1 : a->y1;
    out->y2 = (a->y2 > b->y2) ? b->y2 : a->y2;
    out->x1 &= r_vid.mask;
    out->x2 += ~r_vid.mask;
    out->x2 &= r_vid.mask;
    return out;
}

/* F_19518 @ 0F38:A198 (linear 0x19518, 86 bytes)

   Clip the rectangle (x, y, w, h) against the screen window; result in out */

struct Rect *F_19518(int x, int y, int w, int h, struct Rect *out)
{
    struct Rect rect, scr;

    scr.x1 = ac825et;
    scr.x2 = bu825a->width + ac825et - 1;
    scr.y1 = aw8260y;
    scr.y2 = aw8260y + l_8262 - 1;
    rect.x1 = x;
    rect.x2 = x + w - 1;
    rect.y1 = y;
    rect.y2 = y + h - 1;
    return F_1947A(&scr, &rect, out);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Frame g77bal[12];
struct Seq g_764e[10];
struct Sprite g7d18m;
struct Page aq7d42[2];

/* F_1956E @ 0F38:A1EE (linear 0x1956E, 133 bytes)

   Clip the box (c, d, a, b) to 0..amax x 0..bmax, snap to the r_vid.mask pixel mask; result in out */

void F_1956E(int a, int b, int c, int d, int amax, int bmax, struct Rect *out)
{
    if (d < 0) {
        b += d;
        d = 0;
    }
    if (d + b > bmax) {
        if ((b = bmax - d) < 0)
            b = 0;
    }
    if ((c &= r_vid.mask) < 0) {
        a += c;
        c = 0;
    }
    if (c + a > amax) {
        if ((a = amax - c) < 0)
            a = 0;
    }
    a &= r_vid.mask;
    out->x1 = c;
    out->x2 = c + a - 1;
    out->y1 = d;
    out->y2 = d + b - 1;
}

/* F_195F3 @ 0F38:A273 (linear 0x195F3, 582 bytes)

   Set up the sprite g7d18m from n frame images: the ring of frames, the display pages and the
   clipped rectangle */

struct Sprite *F_195F3(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img,
    struct Bitmap far *save, struct Rect *a8, struct Rect *a9)
{
    int i;

    y -= (*imgs)->height - 1;
    for (i = 0; i < n; i++) {
        g77bal[i].pos = *imgs++;
        if (cell)
            g77bal[i].data = r_vid.mode == -2 ? cell++ : &am6e68x[i];
        else
            g77bal[i].data = 0;
        g77bal[i].nxt[0] = &g77bal[i + 1];
    }
    g7d18m.frame = g77bal[i - 1].nxt[0] = g77bal;
    for (i = 0; i < n; i++)
        g77bal[i].nxt[1] = g77bal[i].nxt[0]->nxt[0];
    for (i = 0; i < n; i++)
        g77bal[i].nxt[2] = g77bal[i].nxt[1]->nxt[1];
    g7d18m.img = img;
    g7d18m.x = x;
    g7d18m.y = y;
    g7d18m.save = save;
    g7d18m.rc[0] = a8;
    g7d18m.rc[1] = a9;
    if (r_vid.mode == -2) {
        for (i = 0; i < 2; i++) {
            aq7d42[i].bm = bp7316[i];
            aq7d42[i].idx = i;
            aq7d42[i].r.x1 = aq7d42[i].r.y1 = 0;
            aq7d42[i].r.x2 = aq7d42[i].r.y2 = -1;
            aq7d42[i].link = &aq7d42[i + 1];
        }
        aq7d42[i - 1].link = aq7d42;
    } else {
        aq7d42[0].bm = r_vid.front;
        aq7d42[0].idx = 0;
        aq7d42[0].r.x1 = aq7d42[0].r.y1 = 0;
        aq7d42[0].r.x2 = aq7d42[0].r.y2 = -1;
        aq7d42[0].link = aq7d42;
    }
    g7d18m.page = aq7d42;
    F_1956E(g7d18m.frame->pos->width,
        g7d18m.frame->pos->height, g7d18m.x, g7d18m.y,
        aq7d42[0].bm->width, aq7d42[0].bm->height, &aq7d42[0].r);
    g7d18m.seq = g_764e;
    return &g7d18m;
}

/* F_19839 @ 0F38:A4B9 (linear 0x19839, 236 bytes)

   Subtract rectangle a from b along one axis into out; NULL when nothing is left */

struct Rect *F_19839(struct Rect *a, struct Rect *b, struct Rect *out, int horiz)
{
    *out = *b;
    if (horiz) {
        if (b->x1 <= a->x1 && b->x2 <= a->x2) {
            if (b->x2 >= a->x1) {
                if (b->x2 <= a->x2)
                    out->x2 = a->x1 - 1;
                else
                    out->x2 = b->x2;
            }
        } else {
            if (a->x2 >= b->x1)
                out->x1 = a->x2 + 1;
            if (b->x2 < a->x2)
                out->x2 = a->x2;
        }
    } else {
        if (b->y1 < a->y1) {
            if (b->y2 >= a->y1)
                out->y2 = a->y1 - 1;
        } else if (b->y1 > a->y1) {
            if (b->y1 <= a->y2)
                out->y1 = a->y2 + 1;
        }
    }
    if (out->x2 < out->x1 || out->y2 < out->y1) {
        out->x1 = out->y1 = 1000;
        out->x2 = out->y2 = -1;
        return 0;
    }
    return out;
}

/* F_19925 @ 0F38:A5A5 (linear 0x19925, 184 bytes)

   Rotate the ring of four frames of f1 sprite and pick the one for the current heading */

void F_19925(struct Sprite *s)
{
    struct Frame *f1, *f2, *f3, *f4;
    int idx;

    s->x = z_8b7c;
    f4 = (f3 = (f2 = (f1 = s->frame)->nxt[0])->nxt[0])->nxt[0];
    f1->nxt[0] = f4;
    f2->nxt[0] = f1;
    f3->nxt[0] = f2;
    f4->nxt[0] = f3;
    for (idx = 0; idx < 4; idx++)
        g77bal[idx].nxt[1] = g77bal[idx].nxt[0]->nxt[0];
    for (idx = 0; idx < 4; idx++)
        g77bal[idx].nxt[2] = g77bal[idx].nxt[1]->nxt[1];
    switch (z_8b7c & 7) {
    case 0:
        break;
    case 6:
        s->frame = f4;
        return;
    case 4:
        s->frame = f3;
        return;
    case 2:
        s->frame = f2;
    }
}

/* F_199DD @ 0F38:A65D (linear 0x199DD, 80 bytes)

   Store step i of a sprite's motion sequence and terminate the sequence after it */

void F_199DD(struct Sprite *s, int i, int count, signed char dx, signed char dy)
{
    s->seq[i].count = count;
    s->seq[i].dx = dx;
    s->seq[i].dy = dy;
    s->seq[i + 1].count = -1;
    s->seq[i + 1].dx = 0;
    s->seq[i + 1].dy = 0;
}

/* F_19A2D @ 0F38:A6AD (linear 0x19A2D, 321 bytes)

   Split a move of size d into the three phases of a sprite's motion sequence */

void F_19A2D(struct Sprite *s, int mode, int a, int b, int c, int d, int e, int f)
{
    int n1, n2, n3;

    switch (mode) {
    case 1:
    case 4:
        n3 = -(e + f - 100) * d / 100 / c & 0xfffc;
        n2 = f * d / 100 / b & 0xfffe;
        n1 = (d - n2 * b - n3 * c) / a;
        break;
    case 2:
    case 6:
        n1 = abs(e * d / 100 / a) & 0xfffc;
        n2 = abs(f * d / 100 / b) & 0xfffe;
        n3 = abs((d - abs(a) * n1 - abs(b) * n2) / c);
        break;
    }
    F_199DD(s, 0, n1 + 1, (char)a, 0);
    F_199DD(s, 1, n2, (char)b, 0);
    F_199DD(s, 2, n3, (char)c, 0);
    F_199DD(s, 3, -1, 0, 0);
}

/* F_19B6E @ 0F38:A7EE (linear 0x19B6E, 144 bytes)

   Build the motion sequence of a sprite for a given direction */

void F_19B6E(struct Sprite *s, int mode, int d)
{
    switch (mode) {
    case 1:
    case 4:
        if (g_0284 == 1)
            F_19A2D(s, 1, 8, 4, 2, d, 0, 0x3c);
        else
            F_19A2D(s, 1, 8, 4, 2, d, 0x19, 0x37);
        break;
    case 6:
        F_19925(s);
        F_19A2D(s, 6, -2, -4, -4, d, 0x28, 0x3c);
        break;
    case 2:
        F_19A2D(s, 2, 2, 4, 8, d, 0xa, 0x3c);
        break;
    }
}

/* F_19BFE @ 0F38:A87E (linear 0x19BFE, 441 bytes)

   Slide a one-frame sprite of bitmap bm from y0 to y1 at column x, refreshing the front page under
   it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void F_19BFE(int a, int b, int x, int y0, int y1, struct Bitmap far *bm)
{
    struct Sprite *sp;
    int n1;
    int dir;
    struct Bitmap far *save;
    struct Bitmap far *img2;
    int old;

    old = z_8b7c;
    n1 = y1 - y0;
    dir = 1;
    if (n1 < 0) {
        n1 = -n1;
        dir = -1;
    }
    n1++;
    img2 = F_11A72(x, 0, bm->width, 200, 0);
    save = F_1934E(img2->width, img2->height, 0, 2);
    F_19ED0(img2, save, 1);
    sp = F_195F3(x, bm->height + y0 - 1, &bm, 0, 1, img2, save, 0, 0);
    F_199DD(sp, 0, n1, 0, (char)dir);
    sp->page->bm = r_vid.back;
    while ((*g78c6c)(sp)) {
        if (g_0792)
            (*ab78aat)(g_0792, r_vid.back, &u_70ac, 1);
        if (r_vid.f16)
            F_1A92A(r_vid.back, r_vid.front, x, a, bm->width + x, b);
    }
    if (r_vid.f16)
        F_1A92A(r_vid.front, r_vid.back, x, (y0 > y1 ? y1 : y0) < 0 ? 0 : (y0 > y1 ? y1 : y0), bm->width + x,
            (y0 < y1 ? y1 : y0) + bm->height > 199 ? 199 : (y0 < y1 ? y1 : y0) + bm->height);
    z_8b7c = old;
    F_19438(2);
}

/* F_19DB7 @ 0F38:AA37 (linear 0x19DB7, 139 bytes)

   Advance a sprite one tick along its motion sequence */

void F_19DB7(struct Sprite *s)
{
    int n;

    z_8b7c = s->x;
    g_0288 = s->frame->pos;
    s->seq->count--;
    if (s->seq->count == 0)
        s->seq++;
    if (s->seq->count != -1) {
        s->x += s->seq->dx;
        s->y += s->seq->dy;
        n = s->seq->dx;
        if (n < 0)
            n = -n;
        if (n)
            s->frame = s->frame->nxt[n >> 2];
    }
}

/* F_19E42 @ 0F38:AAC2 (linear 0x19E42, 142 bytes)

   Clip a span [pos, pos+len) against a sprite's frame, masked to r_vid.mask */

int F_19E42(int pos, int len, struct Sprite *s, struct Blt16 *o)
{
    int a, b, c, d;

    b = s->x & r_vid.mask;
    a = s->frame->pos->width;
    if (pos + len <= b || b + a < pos)
        return 0;
    c = (b < pos) ? pos : b;
    d = (b + a > pos + len) ? pos + len : b + a;
    d = (d + ~r_vid.mask) & r_vid.mask;
    o->sx = (c - b < 0) ? 0 : c - b;
    o->dx = c;
    o->w = d - c;
    return 1;
}
