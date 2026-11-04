#include "STRUCTS.H"
#include "M_23F68.H"
#include "M_00000.H"
#include "M_0914C.H"
#include "M_1947A.H"
#include "M_2322E.H"
#include "M_23262.H"
#include "M_24E78.H"

static void near F_23F68(struct Rect *r, struct Bitmap far *dst, struct Bitmap far *src, int rows);

/* F_23F68 @ 21A0:2568 (linear 0x23F68, 113 bytes)

   Copy a rectangle between two planar bitmaps with F_24E78 */

static void near F_23F68(struct Rect *r, struct Bitmap far *dst, struct Bitmap far *src, int rows)
{
    int off = rows ? 40 * r->y1 : r->x1 >> 3;

    F_24E78(src->bits.p - off, dst->bits.p, (r->x2 >> 3) - (r->x1 >> 3) + 1,
            r->y2 - r->y1 + 1, r->x1 >> 3, r->y1, dst->pitch);
}

/* F_23FD9 @ 21A0:25D9 (linear 0x23FD9, 787 bytes)

   Advance a sprite to its next display page and redraw it there */

int F_23FD9(struct Sprite *s)
{
    struct Rect clip;
    struct Rect first;
    struct Rect f;
    struct Blt16 dest;
    struct Frame *box;      /* current frame */
    struct Page *count;     /* current page */
    struct Bitmap far *height; /* background save */
    struct Bitmap far *key; /* sprite image */
    int i;

    if (s->seq->count < 0) {
        count = s->page;
        F_24E78(count->bm->bits.p, count->link->bm->bits.p, 40, 190, 0, 0, 40);
        if (count->idx) {
            count = count->link;
            count->r = count->link->r;
            F_2322E(0);
        }
        return 0;
    }
    count = s->page = s->page->link;
    box = s->frame;
    key = s->img;
    height = s->save;
    F_1956E(box->pos->width,
        box->pos->height - g_02d0, s->x, s->y,
        count->bm->width, count->bm->height, &clip);
    if (F_19839(&clip, &count->r, &f, s->seq->dx))
        F_23F68(&f, count->bm, key, s->seq->dx);
    count->r = clip;
    dest.h = box->pos->height - g_02d0;
    dest.sy = 0;
    dest.dy = s->y;
    if (g_0284 != 6) {
        dest.w = box->pos->width;
        dest.sx = 0;
        dest.dx = s->x;
        F_234E9(box->pos, count->bm, height, (struct Mask *)box->data, (struct Blt *)&dest);
    } else {
        for (i = 0; i < 6; i++) {
            if (F_19E42(g_539a[i].a, g_539a[i].b, s, &dest))
                F_234E9(box->pos, count->bm, height, (struct Mask *)box->data, (struct Blt *)&dest);
        }
    }
    for (i = 0; i < 2; i++) {
        if (!s->rc[i])
            continue;
        first = *s->rc[i];
        if (box->pos->pitch == height->pitch) {
            first.x1 += s->x;
            first.x2 += s->x;
        } else {
            first.y1 += s->y;
            first.y2 += s->y;
        }
        if (F_1947A(&first, &count->r, &f)) {
            dest.w = f.x2 - f.x1 + 1;
            dest.h = f.y2 - f.y1 + 1;
            if (box->pos->pitch == height->pitch) {
                dest.sx = 0;
                dest.sy = f.y1;
            } else {
                dest.sx = f.x1;
                dest.sy = f.y1 - s->y;
            }
            dest.dx = f.x1;
            dest.dy = f.y1;
            F_236D5(height, count->bm, (struct Blt *)&dest, 0);
        }
    }
    F_19DB7(s);
    F_2322E(count->idx);
    return 1;
}
