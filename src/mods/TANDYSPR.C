#include "STRUCTS.H"
#include "TANDYSPR.H"
#include "BITMAP.H"
#include "M_00000.H"
#include "M_0914C.H"
#include "M_10B7C.H"
#include "M_174A6.H"
#include "M_1947A.H"
#include "M_1A9B2.H"
#include "TANDYBLT.H"

static void near F_226F4(struct Rect *r, struct Bitmap far *a, struct Bitmap far *b, int horiz);

/* F_226F4 @ 21A0:0CF4 (linear 0x226F4, 110 bytes)

   Blit a rectangle between two bitmaps, offset by a scroll origin */

static void near F_226F4(struct Rect *r, struct Bitmap far *a, struct Bitmap far *b, int horiz)
{
    struct Blt blt;
    blt.w = r->x2 - r->x1 + 1;
    blt.h = r->y2 - r->y1 + 1;
    blt.dx = r->x1;
    blt.dy = r->y1;
    if (horiz) {
        blt.sx = r->x1 - g_5a20;
        blt.sy = 0;
    } else {
        blt.sx = 0;
        blt.sy = blt.dy - g_5aa8;
    }
    F_21EAB(b, a, &blt, 1);
}

/* F_22762 @ 21A0:0D62 (linear 0x22762, 965 bytes)

   Erase and redraw one sprite on the screen page r_vid.back through the masked blitters, then flush
   the touched rectangle to the page and advance the sprite */
/* Local names are placeholders chosen to reproduce the original stack layout. */

int F_22762(struct Sprite *s)
{
    struct Blt blt;
    int top, t;
    struct Bitmap *d;
    struct Rect e;
    int u;
    struct Rect rc;
    int idx;
    struct Rect s2;
    struct Rect c2;
    struct Bitmap far *fp;
    struct Page *pg;
    struct Frame *fr;

    if (s->seq->count < 0)
        return 0;
    pg = s->page;
    fr = s->frame;
    fp = s->save;
    d = fr->data;
    F_1956E(fr->pos->width, fr->pos->height - g_02d0,
        s->x, s->y, pg->bm->width, pg->bm->height, &rc);
    if (F_19839(&rc, &pg->r, &c2, s->seq->dx))
        F_226F4(&c2, r_vid.back, fp, s->seq->dx);
    pg->r = rc;
    top = d ? d->height : 0;
    t = blt.h = fr->pos->height - top - g_02d0;
    blt.sy = 0;
    u = blt.dy = s->y;
    if (g_0284 != 6) {
        blt.w = fr->pos->width;
        blt.sx = 0;
        blt.dx = s->x;
        if (s->seq->dx) {
            blt.mx = blt.dx - g_5a20 < 0 ? 0 : blt.dx - g_5a20;
            blt.my = 0;
        } else {
            blt.mx = 0;
            blt.my = blt.dy - g_5aa8 < 0 ? 0 : blt.dy - g_5aa8;
        }
        F_21C0B(fr->pos, r_vid.back, fp, &blt);
        if (d) {
            blt.dy += blt.h;
            blt.h = top;
            F_21EAB((struct Bitmap far *)d, r_vid.back, &blt, 1);
        }
    } else {
        blt.my = 0;
        for (idx = 0; idx < 6; idx++) {
            if (F_19E42(g_539a[idx].a, g_539a[idx].b, s, (struct Blt16 *)&blt)) {
                blt.dy = u;
                blt.h = t;
                blt.mx = blt.dx - g_5a20;
                F_21C0B(fr->pos, r_vid.back, fp, &blt);
                if (d) {
                    blt.dy += t;
                    blt.h = top;
                    F_21EAB((struct Bitmap far *)d, r_vid.back, &blt, 1);
                }
            }
        }
    }
    for (idx = 0; idx < 2; idx++) {
        if (s->rc[idx] == 0)
            continue;
        s2 = *s->rc[idx];
        if (fp->pitch == fr->pos->pitch) {
            s2.x1 += s->x;
            s2.x2 += s->x;
        } else {
            s2.y1 += s->y;
            s2.y2 += s->y;
        }
        if (F_1947A(&s2, &pg->r, &e)) {
            blt.w = e.x2 - e.x1 + 1;
            blt.h = e.y2 - e.y1 + 1;
            blt.dx = e.x1;
            blt.dy = e.y1;
            if (fp->pitch == fr->pos->pitch) {
                blt.sx = 0;
                blt.sy = e.y1 - g_5aa8;
            } else {
                blt.sx = e.x1 - g_5a20;
                blt.sy = e.y1 - s->y;
            }
            (*ab78aat)(fp, r_vid.back, &blt, 0);
        }
    }
    if (r_vid.back->bits.p != pg->bm->bits.p)
        F_1A92A(r_vid.back, pg->bm, rc.x1 > c2.x1 ? c2.x1 : rc.x1, rc.y1 > c2.y1 ? c2.y1 : rc.y1,
            rc.x2 < c2.x2 ? c2.x2 : rc.x2, rc.y2 < c2.y2 ? c2.y2 : rc.y2);
    F_19DB7(s);
    return 1;
}
