#include <conio.h>
#include <stdlib.h>
#include "CGASPR.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CGABLT.H"
#include "DASH.H"
#include "MAIN.H"
#include "PC.H"
#include "SPRITE.H"

static void near restore_rect(struct Rect *rect, struct Bitmap far *bm2, struct Bitmap far *bm1, int flag);

/* Blit the part of one bitmap that lies under a rectangle */

static void near restore_rect(struct Rect *rect, struct Bitmap far *bm2, struct Bitmap far *bm1, int flag)
{
    struct Clip c;

    c.w = rect->x2 - rect->x1 + 1;
    c.h = rect->y2 - rect->y1 + 1;
    c.sx = c.dx = flag ? rect->x1 : 0;
    c.sy = c.dy = flag ? 0 : rect->y1;
    cb_copy(bm1, bm2, (struct Blt *)&c, 1);
}

/* Erase and redraw one sprite: restore the background under its old rectangles, draw it over the
   screen pg and advance it */

int cs_step(struct Sprite *s)
{
    struct Bitmap far *bits;
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
    bits = s->workBitmap;
    fp = s->background;
    d = fr->data;
    sp_clip_box(fr->pos->width, fr->pos->height, s->x, s->y, pg->bm->width, pg->bm->height, &rc);
    if (sp_rect_sub(&rc, &pg->rect, &c2, s->motionStep->dx))
        restore_rect(&c2, bits, fp, s->motionStep->dx);
    pg->rect = rc;
    top = d ? d->height : 0;
    t = blt.h = fr->pos->height - top - g_spriteBottomCut;
    blt.sy = 0;
    u = blt.dy = s->motionStep->dy ? s->y : 0;
    if (g_scene != SCENE_JAIL) {
        blt.w = fr->pos->width;
        blt.sx = 0;
        blt.dx = s->motionStep->dx ? s->x : 0;
        cb_compose(fr->pos, bits, fp, &blt);
        if (d) {
            blt.dy += blt.h;
            blt.h = top;
            cb_copy(d, bits, &blt, 1);
        }
    } else {
        for (idx = 0; idx < 6; idx++) {
            if (sp_clip_span(g_jailGaps[idx].a, g_jailGaps[idx].b, s, (struct Blt16 *)&blt)) {
                blt.dy = u;
                blt.h = t;
                cb_compose(fr->pos, bits, fp, &blt);
                if (d) {
                    blt.dy += t;
                    blt.h = top;
                    cb_copy(d, bits, &blt, 1);
                }
            }
        }
    }
    for (idx = 0; idx < 2; idx++) {
        if (s->overlayRects[idx] == 0)
            continue;
        s2 = *s->overlayRects[idx];
        if (fp->pitch == fr->pos->pitch) {
            s2.x1 += s->x;
            s2.x2 += s->x;
        } else {
            s2.y1 += s->y;
            s2.y2 += s->y;
        }
        if (sp_rect_and(&s2, &pg->rect, &e)) {
            blt.w = e.x2 - e.x1 + 1;
            blt.h = e.y2 - e.y1 + 1;
            if (fp->pitch == fr->pos->pitch) {
                blt.sx = blt.dx = 0;
                blt.sy = blt.dy = e.y1;
            } else {
                blt.sx = blt.dx = e.x1;
                blt.sy = blt.dy = e.y1 - s->y;
            }
            (*g_videoOps.maskedBlit)(fp, bits, &blt, 0);
        }
    }
    blt.w = max(rc.x2, c2.x2) - min(rc.x1, c2.x1) + 1;
    blt.h = max(rc.y2, c2.y2) - min(rc.y1, c2.y1) - g_spriteBottomCut + 1;
    blt.dx = min(rc.x1, c2.x1);
    blt.dy = min(rc.y1, c2.y1);
    blt.sx = s->motionStep->dx ? blt.dx : 0;
    blt.sy = s->motionStep->dy ? blt.dy : 0;
    if (r_vid.mode != VID_HERCULES) {
        while (inp(INPUT_STATUS) & STATUS_VRETRACE)
            ;
        while (!(inp(INPUT_STATUS) & STATUS_VRETRACE))
            ;
    } else {
        while (!(inp(MONO_STATUS) & HERC_STATUS_VSYNC))
            ;
        while (inp(MONO_STATUS) & HERC_STATUS_VSYNC)
            ;
    }
    cb_copy(bits, pg->bm, &blt, 1);
    sp_advance(s);
    return 1;
}
