/* F_1A2D0: mask-blit src into dst, optionally and-ing the pixels together */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1A2D0(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag)
{
    struct Blt a;
    struct Blt e;
    struct Bitmap far *j;
    char far *c;
    char far *g;
    int f, b, h, d, i, k, l, m;

    for (i = 0; i < 10; i++)
        ((int *)&e)[i] = ((int *)r)[i];
    f = r->dx & ~g_vid.mask;
    if (f > 0) {
        j = F_1934E(dst->width + 8, dst->height, dst->alt.p != 0L, 2);
        F_1A03F(dst, f, j);
        e.w = j->width;
    } else
        j = dst;
    (*g_78aa)(j, src, &e, 1);
    if (flag) {
        if (F_10A00(j, src, &e, &a)) {
            d = j->pitch;
            b = a.w;
            h = a.h;
            l = d - b;
            m = src->pitch - b;
            c = j->alt.p + a.sy * d + a.sx;
            g = src->alt.p + src->pitch * a.dy + a.dx;
            for (i = 0; i < h; i++) {
                for (k = 0; k < b; k++) {
                    *g &= *c;
                    g++;
                    c++;
                }
                c += l;
                g += m;
            }
        }
    }
    if (f)
        F_19438(1);
}
