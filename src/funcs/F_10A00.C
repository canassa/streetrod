/* F_10A00 @ 0F38:1680 (linear 0x10A00, 253 bytes)

   Clips the rectangle in to the destination bitmap, then to the
   source bitmap; the result goes to out. Returns 1 if anything is left. The x values of in
   are shifted down by g_vid.shift. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

int F_10A00(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out)
{
    int t1, t2;
    int w1, h;
    int sx, sy, dx, dy;

    sx = in->sx >> g_vid.shift;
    sy = in->sy;
    w1 = in->w >> g_vid.shift;
    h = in->h;
    if (sx + w1 > dst->pitch)
        w1 = dst->pitch - sx;
    if (sy + h > dst->height)
        h = dst->height - sy;
    dx = in->dx >> g_vid.shift;
    dy = in->dy;
    if (dy < 0) {
        h += dy;
        sy -= dy;
        dy = 0;
    }
    if ((t1 = src->height) < dy + h)
        h = t1 - dy;
    if (dx < 0) {
        w1 += dx;
        sx -= dx;
        dx = 0;
    }
    if ((t2 = src->pitch) < dx + w1)
        w1 = t2 - dx;
    out->sx = sx;
    out->sy = sy;
    out->dx = dx;
    out->dy = dy;
    out->w = w1;
    out->h = h;
    return out->w > 0 && out->h > 0;
}
