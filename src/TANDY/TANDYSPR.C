#include "TANDYSPR.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "DASH.H"
#include "MAIN.H"
#include "SPRITE.H"
#include "TANDYBLT.H"
#include "TIRES.H"

static void near restore_rect_back(struct Rect *r, struct Bitmap far *a, struct Bitmap far *b, int horiz);

/* Blit a rectangle between two bitmaps, offset by a scroll origin */

static void near restore_rect_back(struct Rect *r, struct Bitmap far *a, struct Bitmap far *b, int horiz)
{
    struct Blt blt;
    blt.w = r->x2 - r->x1 + 1;
    blt.h = r->y2 - r->y1 + 1;
    blt.dx = r->x1;
    blt.dy = r->y1;
    if (horiz) {
        blt.sx = r->x1 - g_backdropX;
        blt.sy = 0;
    } else {
        blt.sx = 0;
        blt.sy = blt.dy - g_spriteBgY;
    }
    tb_copy(b, a, &blt, 1);
}

/* Erase and redraw one sprite on the screen page r_vid.back through the masked blitters, then flush
   the touched rectangle to the page and advance the sprite */

int ts_step(struct Sprite *s)
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

    if (s->motionStep->count < 0)
        return 0;
    pg = s->page;
    fr = s->frame;
    fp = s->background;
    d = fr->bottomBand;
    sp_clip_box(fr->image->width, fr->image->height - g_spriteBottomCut, s->x, s->y, pg->bm->width, pg->bm->height,
                &rc);
    if (sp_rect_sub(&rc, &pg->rect, &c2, s->motionStep->dx))
        restore_rect_back(&c2, r_vid.back, fp, s->motionStep->dx);
    pg->rect = rc;
    top = d ? d->height : 0;
    t = blt.h = fr->image->height - top - g_spriteBottomCut;
    blt.sy = 0;
    u = blt.dy = s->y;
    if (g_scene != SCENE_JAIL) {
        blt.w = fr->image->width;
        blt.sx = 0;
        blt.dx = s->x;
        if (s->motionStep->dx) {
            blt.mx = blt.dx - g_backdropX < 0 ? 0 : blt.dx - g_backdropX;
            blt.my = 0;
        } else {
            blt.mx = 0;
            blt.my = blt.dy - g_spriteBgY < 0 ? 0 : blt.dy - g_spriteBgY;
        }
        tb_compose(fr->image, r_vid.back, fp, &blt);
        if (d) {
            blt.dy += blt.h;
            blt.h = top;
            tb_copy((struct Bitmap far *)d, r_vid.back, &blt, 1);
        }
    } else {
        blt.my = 0;
        for (idx = 0; idx < 6; idx++) {
            if (sp_clip_span(g_jailGaps[idx].a, g_jailGaps[idx].b, s, (struct Blt16 *)&blt)) {
                blt.dy = u;
                blt.h = t;
                blt.mx = blt.dx - g_backdropX;
                tb_compose(fr->image, r_vid.back, fp, &blt);
                if (d) {
                    blt.dy += t;
                    blt.h = top;
                    tb_copy((struct Bitmap far *)d, r_vid.back, &blt, 1);
                }
            }
        }
    }
    for (idx = 0; idx < 2; idx++) {
        if (s->overlayRects[idx] == 0)
            continue;
        s2 = *s->overlayRects[idx];
        if (fp->pitch == fr->image->pitch) {
            s2.x1 += s->x;
            s2.x2 += s->x;
        } else {
            s2.y1 += s->y;
            s2.y2 += s->y;
        }
        if (sp_rect_and(&s2, &pg->rect, &e)) {
            blt.w = e.x2 - e.x1 + 1;
            blt.h = e.y2 - e.y1 + 1;
            blt.dx = e.x1;
            blt.dy = e.y1;
            if (fp->pitch == fr->image->pitch) {
                blt.sx = 0;
                blt.sy = e.y1 - g_spriteBgY;
            } else {
                blt.sx = e.x1 - g_backdropX;
                blt.sy = e.y1 - s->y;
            }
            (*g_videoOps.maskedBlit)(fp, r_vid.back, &blt, 0);
        }
    }
    if (r_vid.back->bits.p != pg->bm->bits.p)
        bm_copy_box(r_vid.back, pg->bm, rc.x1 > c2.x1 ? c2.x1 : rc.x1, rc.y1 > c2.y1 ? c2.y1 : rc.y1,
                    rc.x2 < c2.x2 ? c2.x2 : rc.x2, rc.y2 < c2.y2 ? c2.y2 : rc.y2);
    sp_advance(s);
    return 1;
}
