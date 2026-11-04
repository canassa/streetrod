#include "STRUCTS.H"
#include "EGASPR.H"
#include "BOBS.H"
#include "EGABLIT.H"
#include "EGALATCH.H"
#include "EGAPAGE.H"
#include "MAIN.H"
#include "PC.H"
#include "SCREEN.H"
#include "SPRITE.H"

static void near restore_rect_latch(struct Rect *r, struct Bitmap far *dst, struct Bitmap far *src, int rows);

/* restore_rect_latch @ 21A0:2568 (linear 0x23F68, 113 bytes)

   Copy a rectangle between two planar bitmaps with egalatch_rect */

static void near restore_rect_latch(struct Rect *r, struct Bitmap far *dst, struct Bitmap far *src, int rows)
{
    int off = rows ? EGA_PITCH * r->y1 : r->x1 >> 3;

    egalatch_rect(src->bits.p - off, dst->bits.p, (r->x2 >> 3) - (r->x1 >> 3) + 1,
            r->y2 - r->y1 + 1, r->x1 >> 3, r->y1, dst->pitch);
}

/* egaspr_step @ 21A0:25D9 (linear 0x23FD9, 787 bytes)

   Advance a sprite to its next display page and redraw it there */

int egaspr_step(struct Sprite *s)
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
        egalatch_rect(count->bm->bits.p, count->link->bm->bits.p, EGA_PITCH, SCREEN_BODY_HEIGHT, 0, 0, EGA_PITCH);
        if (count->idx) {
            count = count->link;
            count->r = count->link->r;
            egapage_show(0);
        }
        return 0;
    }
    count = s->page = s->page->link;
    box = s->frame;
    key = s->img;
    height = s->save;
    sprite_clip_box(box->pos->width,
        box->pos->height - g_spriteBottomCut, s->x, s->y,
        count->bm->width, count->bm->height, &clip);
    if (sprite_rect_sub(&clip, &count->r, &f, s->seq->dx))
        restore_rect_latch(&f, count->bm, key, s->seq->dx);
    count->r = clip;
    dest.h = box->pos->height - g_spriteBottomCut;
    dest.sy = 0;
    dest.dy = s->y;
    if (g_scene != 6) {
        dest.w = box->pos->width;
        dest.sx = 0;
        dest.dx = s->x;
        egablit_sprite(box->pos, count->bm, height, (struct Mask *)box->data, (struct Blt *)&dest);
    } else {
        for (i = 0; i < 6; i++) {
            if (sprite_clip_span(g_jailGaps[i].a, g_jailGaps[i].b, s, &dest))
                egablit_sprite(box->pos, count->bm, height, (struct Mask *)box->data, (struct Blt *)&dest);
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
        if (sprite_rect_and(&first, &count->r, &f)) {
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
            egablit_masked(height, count->bm, (struct Blt *)&dest, 0);
        }
    }
    sprite_advance(s);
    egapage_show(count->idx);
    return 1;
}
