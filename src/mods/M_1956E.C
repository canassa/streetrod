#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <stdlib.h>

/* F_1956E: clip the box (c, d, a, b) to 0..amax x 0..bmax, snap to the g_vid.mask pixel mask; result in out */


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
    if ((c &= g_vid.mask) < 0) {
        a += c;
        c = 0;
    }
    if (c + a > amax) {
        if ((a = amax - c) < 0)
            a = 0;
    }
    a &= g_vid.mask;
    out->x1 = c;
    out->x2 = c + a - 1;
    out->y1 = d;
    out->y2 = d + b - 1;
}

/* F_195F3: set up the sprite g_7d18 from n frame images: the ring of frames, the display pages and the clipped rectangle */


struct Sprite *F_195F3(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img,
    struct Bitmap far *save, struct Rect *a8, struct Rect *a9)
{
    int i;

    y -= (*imgs)->height - 1;
    for (i = 0; i < n; i++) {
        g_77ba[i].pos = (long)*imgs++;
        if (cell)
            g_77ba[i].data = g_vid.mode == -2 ? cell++ : &g_6e68[i];
        else
            g_77ba[i].data = 0;
        g_77ba[i].nxt[0] = &g_77ba[i + 1];
    }
    g_7d18.frame = g_77ba[i - 1].nxt[0] = g_77ba;
    for (i = 0; i < n; i++)
        g_77ba[i].nxt[1] = g_77ba[i].nxt[0]->nxt[0];
    for (i = 0; i < n; i++)
        g_77ba[i].nxt[2] = g_77ba[i].nxt[1]->nxt[1];
    g_7d18.img = img;
    g_7d18.x = x;
    g_7d18.y = y;
    g_7d18.save = save;
    g_7d18.rc[0] = a8;
    g_7d18.rc[1] = a9;
    if (g_vid.mode == -2) {
        for (i = 0; i < 2; i++) {
            g_7d42[i].bm = g_7316[i];
            g_7d42[i].idx = i;
            g_7d42[i].r.x1 = g_7d42[i].r.y1 = 0;
            g_7d42[i].r.x2 = g_7d42[i].r.y2 = -1;
            g_7d42[i].link = &g_7d42[i + 1];
        }
        g_7d42[i - 1].link = g_7d42;
    } else {
        g_7d42[0].bm = g_vid.front;
        g_7d42[0].idx = 0;
        g_7d42[0].r.x1 = g_7d42[0].r.y1 = 0;
        g_7d42[0].r.x2 = g_7d42[0].r.y2 = -1;
        g_7d42[0].link = g_7d42;
    }
    g_7d18.page = g_7d42;
    F_1956E(((struct Bitmap far *)g_7d18.frame->pos)->width,
        ((struct Bitmap far *)g_7d18.frame->pos)->height, g_7d18.x, g_7d18.y,
        g_7d42[0].bm->width, g_7d42[0].bm->height, &g_7d42[0].r);
    g_7d18.seq = g_764e;
    return &g_7d18;
}

/* F_19839: subtract rectangle a from b along one axis into out; NULL when nothing is left */


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

/* F_19925: rotate the ring of four frames of f1 sprite and pick the one for the current heading */


void F_19925(struct Sprite *s)
{
    struct Frame *f1, *f2, *f3, *f4;
    int idx;

    s->x = g_8b7c;
    f4 = (f3 = (f2 = (f1 = s->frame)->nxt[0])->nxt[0])->nxt[0];
    f1->nxt[0] = f4;
    f2->nxt[0] = f1;
    f3->nxt[0] = f2;
    f4->nxt[0] = f3;
    for (idx = 0; idx < 4; idx++)
        g_77c0[idx].f2 = g_77c0[idx].f0->nxt[0];
    for (idx = 0; idx < 4; idx++)
        g_77c0[idx].f4 = g_77c0[idx].f2->nxt[1];
    switch (g_8b7c & 7) {
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

/* F_199DD: store step i of a sprite's motion sequence and terminate the sequence after it */


void F_199DD(struct Sprite *s, int i, int count, signed char dx, signed char dy)
{
    s->seq[i].count = count;
    s->seq[i].dx = dx;
    s->seq[i].dy = dy;
    s->seq[i + 1].count = -1;
    s->seq[i + 1].dx = 0;
    s->seq[i + 1].dy = 0;
}

/* F_19A2D: split a move of size d into the three phases of a sprite's motion sequence */


#pragma intrinsic(abs)

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

/* F_19B6E: build the motion sequence of a sprite for a given direction */


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

/* F_19BFE: slide a one-frame sprite of bitmap bm from y0 to y1 at column x, refreshing the front page under it */
/* Local names are placeholders chosen to reproduce the original stack layout. */


void F_19BFE(int a, int b, int x, int y0, int y1, struct Bitmap far *bm)
{
    struct Sprite *sp;
    int n1;
    int dir;
    struct Bitmap far *save;
    struct Bitmap far *img2;
    int old;

    old = g_8b7c;
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
    sp->page->bm = g_vid.back;
    while ((*g_78c6)(sp)) {
        if (g_0792)
            (*g_78aa)(g_0792, g_vid.back, &g_70ac, 1);
        if (g_vid.f16)
            F_1A92A(g_vid.back, g_vid.front, x, a, bm->width + x, b);
    }
    if (g_vid.f16)
        F_1A92A(g_vid.front, g_vid.back, x, (y0 > y1 ? y1 : y0) < 0 ? 0 : (y0 > y1 ? y1 : y0), bm->width + x,
            (y0 < y1 ? y1 : y0) + bm->height > 199 ? 199 : (y0 < y1 ? y1 : y0) + bm->height);
    g_8b7c = old;
    F_19438(2);
}
