#include <conio.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "CGASPR.H"
#include "BOBS.H"
#include "CGABLT.H"
#include "MAIN.H"
#include "PC.H"
#include "SPRITE.H"
#include "VIDEO.H"

static void near restore_rect(struct Rect *rect, struct Bitmap far *bm2, struct Bitmap far *bm1, int flag);

/* restore_rect @ 1C11:17B4 (linear 0x1D8C4, 97 bytes)

   Blit the part of one bitmap that lies under a rectangle */

static void near restore_rect(struct Rect *rect, struct Bitmap far *bm2, struct Bitmap far *bm1, int flag)
{
    struct Clip c;

    c.w = rect->x2 - rect->x1 + 1;
    c.h = rect->y2 - rect->y1 + 1;
    c.sx = c.dx = flag ? rect->x1 : 0;
    c.sy = c.dy = flag ? 0 : rect->y1;
    cgablt_copy(bm1, bm2, (struct Blt *)&c, 1);
}

/* cgaspr_step @ 1C11:1815 (linear 0x1D925, 1021 bytes)

   Erase and redraw one sprite: restore the background under its old rectangles, draw it over the
   screen pg and advance it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

int cgaspr_step(struct Sprite *s)
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

    if (s->seq->count < 0)
        return 0;
    pg = s->page;
    fr = s->frame;
    bits = s->img;
    fp = s->save;
    d = fr->data;
    sprite_clip_box(fr->pos->width, fr->pos->height,
        s->x, s->y, pg->bm->width, pg->bm->height, &rc);
    if (sprite_rect_sub(&rc, &pg->r, &c2, s->seq->dx))
        restore_rect(&c2, bits, fp, s->seq->dx);
    pg->r = rc;
    top = d ? d->height : 0;
    t = blt.h = fr->pos->height - top - g_spriteBottomCut;
    blt.sy = 0;
    u = blt.dy = s->seq->dy ? s->y : 0;
    if (g_scene != 6) {
        blt.w = fr->pos->width;
        blt.sx = 0;
        blt.dx = s->seq->dx ? s->x : 0;
        cgablt_compose(fr->pos, bits, fp, &blt);
        if (d) {
            blt.dy += blt.h;
            blt.h = top;
            cgablt_copy(d, bits, &blt, 1);
        }
    } else {
        for (idx = 0; idx < 6; idx++) {
            if (sprite_clip_span(g_jailGaps[idx].a, g_jailGaps[idx].b, s, (struct Blt16 *)&blt)) {
                blt.dy = u;
                blt.h = t;
                cgablt_compose(fr->pos, bits, fp, &blt);
                if (d) {
                    blt.dy += t;
                    blt.h = top;
                    cgablt_copy(d, bits, &blt, 1);
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
        if (sprite_rect_and(&s2, &pg->r, &e)) {
            blt.w = e.x2 - e.x1 + 1;
            blt.h = e.y2 - e.y1 + 1;
            if (fp->pitch == fr->pos->pitch) {
                blt.sx = blt.dx = 0;
                blt.sy = blt.dy = e.y1;
            } else {
                blt.sx = blt.dx = e.x1;
                blt.sy = blt.dy = e.y1 - s->y;
            }
            (*ab78aat)(fp, bits, &blt, 0);
        }
    }
    blt.w = max(rc.x2, c2.x2) - min(rc.x1, c2.x1) + 1;
    blt.h = max(rc.y2, c2.y2) - min(rc.y1, c2.y1) - g_spriteBottomCut + 1;
    blt.dx = min(rc.x1, c2.x1);
    blt.dy = min(rc.y1, c2.y1);
    blt.sx = s->seq->dx ? blt.dx : 0;
    blt.sy = s->seq->dy ? blt.dy : 0;
    if (r_vid.mode != -4) {
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
    cgablt_copy(bits, pg->bm, &blt, 1);
    sprite_advance(s);
    return 1;
}
