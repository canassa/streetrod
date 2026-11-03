/* F_17BE6: load the bitmap of car part idx into out[0], paint its marks, and build the n frame copies out[1..] */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_17BE6(struct Bitmap far **out, int idx, int flags, int kk, int q, int *ids, int w, int n)
{
    int row8;
    struct Blt blt;
    struct Spr12 *p12;
    struct Pos6 *pp8;
    int h;
    int off;
    int left;
    struct Model *m2;
    int right1;
    struct Spr6 *p69;
    int j5;
    struct Bitmap far *bm;
    struct Bitmap far *bm2n;

    m2 = &g_8df0[idx];
    p12 = &g_7682[idx];
    p69 = &g_818c[idx];
    *out = F_15BBE(g_7d8e[idx].id, 1);
    g_826e = p69->b0;
    g_8270 = p69->b1;
    g_826a = p69->b2;
    g_826c = p69->b3;
    g_8264 = p69->b2 && p69->b3;
    g_8266 = p69->b4;
    g_8268 = p69->b5;
    g_8276 = ~g_vid.mask & p12->b10;
    g_8272 = g_vid.mask & p12->b10;
    g_8274 = p12->b11;
    if (flags & 2)
        F_1A6E0(*out, m2->f6, 0, p12->b7);
    if (flags & 1)
        F_1A6E0(*out, m2->f8, p12->b8, p12->b9);
    if (flags & 4) {
        F_1A6E0(*out, m2->f2, p12->b4, 0);
        for (j5 = 0; j5 < g_70a8->planesize / g_vid.planes; j5++)
            if (((unsigned char far *)g_70a8->alt.p)[j5] != 0xff)
                break;
        row8 = j5 / g_70a8->pitch;
    } else
        row8 = 0;
    if ((flags & 8) && idx != 0x19)
        F_1A734(*out, m2->f4, p12->b5, p12->b6, 1);
    blt.w = g_0294->width;
    blt.h = g_0294->height;
    blt.sx = p12->b10;
    blt.sy = p12->b11;
    blt.dx = blt.dy = 0;
    (*g_78a2)(*out, g_0294, &blt, 1);
    if (kk > 0) {
        pp8 = &g_7844[kk];
        F_1A734(*out, pp8->id, p12->b10 + pp8->dx, p12->b11 + pp8->dy, 0);
    }
    if (q) {
        bm = F_15BBE(g_8280, 2);
        F_1A472(bm, *out, p12->b2, p12->b3 - bm->height + row8 + 1);
        F_19438(1);
    }
    j5 = 1;
    for (j5 = 1; j5 < n; j5++)
        out[j5] = F_1934E((*out)->width + (g_vid.mode == -6 ? 0 : 8), (*out)->height,
            g_vid.mode != -6, 1);
    if (n > 1) {
        switch (g_vid.mode) {
        case -2:
            for (j5 = 1; j5 < n; j5++)
                F_1A03F(out[j5 - 1], 2, out[j5]);
            break;
        case -6:
            for (j5 = 1; j5 < n; j5++) {
                F_19ED0(out[0], out[j5], 1);
                out[j5]->alt.p = out[0]->alt.p;
            }
            break;
        default:
            F_1A03F(out[0], 2, out[1]);
            F_19ED0(out[0], out[2], 3);
            F_19ED0(out[1], out[3], 3);
            break;
        }
    }
    if (ids) {
        for (j5 = 0; j5 < n; j5++) {
            bm = F_15BBE(ids[j5], 2);
            h = out[j5]->height - bm->height;
            switch (g_vid.mode) {
            case -2:
                off = j5 * 2;
                break;
            case -6:
                off = 0;
                g_5aa6 = n - 1 > j5;
                break;
            default:
                off = (j5 & 1) << 1;
                break;
            }
            F_1A472(bm, out[j5], p12->b0 + off, h);
            F_1A472(bm, out[j5], p12->b1 + off, h);
            F_19438(1);
        }
    }
    if (w) {
        bm2n = F_15BBE(0x49d, 2);
        F_19F42(*out, &left, &right1);
        h = (*out)->height - bm2n->height;
        for (j5 = 0; j5 < n; j5++) {
            switch (g_vid.mode) {
            case -2:
                off = j5 * 2;
                break;
            case -6:
                off = 0;
                g_5aa6 = n - 1 > j5;
                break;
            default:
                off = (j5 & 1) << 1;
                break;
            }
            F_1A472(bm2n, out[j5], left + off, h);
            F_1A472(bm2n, out[j5], right1 - bm2n->width + off + 1, h);
        }
        F_19438(1);
    }
}
