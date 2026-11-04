#include <conio.h>
#include <dos.h>
#include "STRUCTS.H"
#include "M_174A6.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_036A8.H"
#include "M_039C0.H"
#include "M_06476.H"
#include "M_0914C.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_15396.H"
#include "M_18BDC.H"
#include "M_1923C.H"
#include "M_193D4.H"
#include "M_1947A.H"
#include "M_19ED0.H"
#include "M_23262.H"
#include "M_24E78.H"
#include "M_25954.H"

#pragma intrinsic(outp)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Rect g_6cbc;                 /* DS:6CBC */
static void near F_174A6(char far *src, char far *dst, char far *mask, int w, int h, int n, int step);
static void near F_1751A(struct Band *A, struct Bitmap far **B, int n, int y2, int h, char far *mask, char c);
static void near F_176B6(struct Bitmap far **B, int n, int y2, int h, int color);
static void near F_17AAE(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap am6e68x[12];
struct Bitmap far *at70a8;
int (far *g78c6c)(struct Sprite *);
struct Band dz7ec2[4];
struct Bitmap bw7fa8;
int l_8262;

int g_58e2 = 1;   /* DS:58E2 */
int g_58e4 = 0;   /* DS:58E4 */
int g_58e6 = 0;   /* DS:58E6 */
int g_58e8[3] = {1001, 1002, 1000};   /* DS:58E8 */
int g_58ee = 0;   /* DS:58EE */
struct Cmd g_58f0[44] = {{1, 0, -1, -1}, {6, 0, 0, 0}, {2, 0, -1, -1}, {10, 0, 8, 1}, {9, 2, 48, 111}, {5, 0, 250, -1}, {11, 0, 10, 7}, {5, 0, 0, -1}, {11, 4, 10, 7}, {6, 0, 0, 0}, {4, 0, -1, -1}, {6, 0, 0, 0}, {3, 0, -1, -1}, {8, 0, 1233, 2}, {10, 0, 48, 64}, {9, 2, 200, 81}, {13, 0, -1, -1}, {11, 0, 16, 0}, {2, 1, 0, 0}, {11, 4, 16, 0}, {2, 1, 48, 0}, {11, 4, 16, 0}, {2, 1, 96, 0}, {11, 4, 20, 0}, {6, 0, 0, 0}, {5, 0, -1, -1}, {6, 0, 0, 0}, {6, 0, -1, -1}, {6, 0, 0, 0}, {7, 0, -1, -1}, {6, 0, 0, 0}, {8, 0, -1, -1}, {8, 0, 1075, 2}, {10, 0, 16, 94}, {9, 2, 160, 52}, {11, 0, 4, 0}, {2, 1, 0, 0}, {11, 4, 4, 0}, {2, 1, 16, 0}, {11, 4, 4, 0}, {2, 1, 32, 0}, {11, 4, 4, 36}, {6, 0, 0, 0}, {6, 0, 0, 0}};   /* DS:58F0 */
int g_59f8[9] = {8, 0, 2, 12, 10, 25, 27, 29, 31};   /* DS:59F8 */
struct Bitmap far *g_5a0a = (struct Bitmap far *)&bw7fa8;   /* DS:5A0A */
int g_5a0e = 0;   /* DS:5A0E */
struct Rect g_5a10 = {0, 0, 80, 0};   /* DS:5A10 */
struct Rect g_5a18 = {88, 0, 136, 0};   /* DS:5A18 */
int g_5a20 = 0;   /* DS:5A20 */
int g_5a22 = 0;   /* DS:5A22 */
int g_5a24 = 0;   /* DS:5A24 */

/* F_174A6 @ 0F38:8126 (linear 0x174A6, 116 bytes)

   Blit one 4-plane EGA image row set, one plane at a time via the sequencer map mask */

static void near F_174A6(char far *src, char far *dst, char far *mask, int w, int h, int n, int step)
{
    int plane, bit1;

    for (bit1 = 1, plane = 0; bit1 < 9; bit1 <<= 1, plane++, src += step) {
        outp(0x3c4, 2);
        outp(0x3c5, bit1);
        outp(0x3ce, 4);
        outp(0x3cf, plane);
        F_25954(dst, FP_SEG(src), FP_OFF(src), FP_OFF(mask), n, w, h, w);
    }
    outp(0x3c4, 2);
    outp(0x3c5, 0xf);
}

/* F_1751A @ 0F38:819A (linear 0x1751A, 412 bytes)

   For each of n bitmaps, lay a band of h rows into the scratch bitmap bp7316[2], fill or copy it,
   then blit the bitmap through the EGA planes */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near F_1751A(struct Band *A, struct Bitmap far **B, int n, int y2, int h,
    char far *mask, char c)
{
    int i;
    int j;
    char far *base;

    base = bp7316[2]->bits.p + 0x46 * bp7316[2]->pitch;
    for (i = 0; i < n; i++) {
        A[i].p = base;
        A[i].y = B[i]->height - y2 - h;
        A[i].h = h;
        base += B[i]->pitch * h;
        if (mask)
            F_2382D(mask, B[i]->pitch, h, A[i].p, 0x28);
        else {
            outp(0x3ce, 5);
            outp(0x3cf, 2);
            for (j = 0; j < B[i]->pitch * h; j++)
                A[i].p[j] = c;
            outp(0x3ce, 5);
            outp(0x3cf, 0);
        }
        j = A[i].y * B[i]->pitch;
        F_174A6(B[i]->bits.p + j, A[i].p, B[i]->alt.p + j, B[i]->pitch, h, B[i]->pitch,
            B[i]->height * B[i]->pitch);
    }
}

/* F_176B6 @ 0F38:8336 (linear 0x176B6, 392 bytes)

   Copy n bitmaps into the table am6e68x, each filled with a colour pattern and h rows high, in
   space cut from the scratch bitmap bp7316[2] */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near F_176B6(struct Bitmap far **B, int n, int y2, int h, int color)
{
    struct Blt pkt1;
    struct Bitmap far *bm1;
    int v;
    int i;
    char far *base2;
    int H5;
    int maxw;

    base2 = bp7316[2]->bits.p + 0x46 * bp7316[2]->pitch;
    H5 = B[0]->height - y2 - h;
    v = (r_vid.mode == -6 ? color << 4 : color << 2 | color << 4 | color << 6) | color;
    maxw = B[0]->width;
    for (i = 1; i < n; i++)
        if (B[i]->width > maxw)
            maxw = B[i]->width;
    bm1 = F_1934E(maxw, h, 0, 2);
    pkt1.w = maxw;
    pkt1.h = h;
    pkt1.sy = H5;
    i = pkt1.sx = pkt1.dx = pkt1.dy = 0;
    for (i = 0; i < n; i++) {
        am6e68x[i] = *bm1;
        am6e68x[i].bits.p = base2;
        base2 += bm1->planesize;
        F_109C0(bm1->bits.p, v, bm1->planesize);
        (*ab78aat)(B[i], bm1, &pkt1, 1);
        F_19ED0(bm1, &am6e68x[i], 1);
    }
    F_19438(1);
}

/* F_1783E @ 0F38:84BE (linear 0x1783E, 624 bytes)

   Make a copy of a bitmap header that views the rows below y0, plus a bitmap to hold them (*pa), a
   second one (*pb), and two clip rects */

void F_1783E(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb,
    int y1, int y0, struct Rect **pr1, struct Rect **pr2)
{
    struct Bitmap far *p2;
    struct Blt blt;
    int h, diff, flag, size, y2;
    struct Bitmap far *start;

    size = src->pitch;
    y2 = y0;
    h = y1 - y2;
    flag = mode != 0;
    start = (struct Bitmap far *)F_192AC(0x30, 1);
    *start = *src;
    start->bits.w.off += h * size;
    start->height = y2;
    start->planesize = y2 * size * r_vid.planes;
    if (r_vid.mode == -6) {
        if (g_5a22 == 0)
            g_5a22 = src->width;
        *pa = F_1934E(g_5a22, y2, flag, 1);
        blt.w = g_5a22;
        blt.h = y2;
        blt.sx = g_5a20;
        blt.sy = blt.dx = blt.dy = 0;
        (*b_78a2)(start, *pa, &blt, 1);
    } else {
        *pa = F_1934E(src->width, y2, flag, 1);
        F_19ED0(start, *pa, 1);
    }
    switch (r_vid.mode) {
    case -2:
        *pb = start;
        break;
    case -6:
        *pb = 0;
        break;
    default:
        *pb = F_1934E(start->width, y2, 0, 1);
        if (g_0284 == 6)
            F_19ED0(start, *pb, 1);
        break;
    }
    *pr1 = 0;
    *pr2 = 0;
    if (flag) {
        p2 = F_15BBE(mode, 2);
        if (r_vid.mode == -6) {
            blt.sy = p2->height - y2;
            (*b_78a2)(p2, *pa, &blt, 2);
        } else {
            diff = p2->planesize / r_vid.planes - (*pa)->planesize / r_vid.planes;
            F_1A882(p2->alt.p + diff, (*pa)->alt.p, (*pa)->planesize / r_vid.planes);
        }
        F_19438(1);
        *pr1 = &g_5a10;
        (*pr1)->y2 = (*pa)->height - 1;
        *pr2 = &g_5a18;
        (*pr2)->y1 = (*pa)->height - 15;
        (*pr2)->y2 = (*pa)->height - 1;
    }
}

/* F_17AAE @ 0F38:872E (linear 0x17AAE, 312 bytes)

   Render a 16x70 strip into a scratch bitmap with two draw passes and blit it onto two bitmaps */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near F_17AAE(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2)
{
    char far *save1;
    struct Bitmap far *tmp1;
    struct Blt b;
    int t;

    g_4f0e = 0;
    tmp1 = F_1934E(0x10, 0x46, 1, 2);
    F_1592E(id1, tmp1);
    save1 = tmp1->bits.p;
    tmp1->bits.p = 0;
    F_1592E(id2, tmp1);
    tmp1->bits.p = save1;
    g_4f0e = 1;
    F_19ED0(tmp1, dstbm, 1);
    b.h = bu825a->height;
    b.sy = 0;
    b.dy = top - bu825a->height - y + 1;
    t = x - (sx0 & 0xfff8);
    b.sx = (t < 0) ? 0 : t;
    b.w = (t + 16 > 16) ? 16 : t + 16;
    b.dx = (-t < 0) ? 0 : -t;
    (*ab78aat)(bu825a, dstbm, &b, 1);
    b.w = 0x10;
    b.h = 0x46;
    b.dx = b.dy = b.sx = b.sy = 0;
    (*ab78aat)(tmp1, dstbm, &b, 0);
    F_19438(1);
}

/* F_17BE6 @ 0F38:8866 (linear 0x17BE6, 1506 bytes)

   Load the bitmap of car part idx into out[0], paint its marks, and build the n frame copies
   out[1..] */

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

    m2 = &w_8df0[idx];
    p12 = &g7682n[idx];
    p69 = &ap818c[idx];
    *out = F_15BBE(g7d86n[idx].f8, 1);
    ar826e = p69->b0;
    q_8270 = p69->b1;
    g826af = p69->b2;
    ae826cv = p69->b3;
    h_8264 = p69->b2 && p69->b3;
    d_8266 = p69->b4;
    ap8268t = p69->b5;
    m_8272.m8276 = ~r_vid.mask & p12->b10;
    m_8272.m8272 = r_vid.mask & p12->b10;
    m_8272.m8274 = p12->b11;
    if (flags & 2)
        F_1A6E0(*out, m2->f6, 0, p12->b7);
    if (flags & 1)
        F_1A6E0(*out, m2->f8, p12->b8, p12->b9);
    if (flags & 4) {
        F_1A6E0(*out, m2->f2, p12->b4, 0);
        for (j5 = 0; j5 < at70a8->planesize / r_vid.planes; j5++)
            if (((unsigned char far *)at70a8->alt.p)[j5] != 0xff)
                break;
        row8 = j5 / at70a8->pitch;
    } else
        row8 = 0;
    if ((flags & 8) && idx != 0x19)
        F_1A734(*out, m2->f4, p12->b5, p12->b6, 1);
    blt.w = g_0294->width;
    blt.h = g_0294->height;
    blt.sx = p12->b10;
    blt.sy = p12->b11;
    blt.dx = blt.dy = 0;
    (*b_78a2)(*out, g_0294, &blt, 1);
    if (kk > 0) {
        pp8 = &a784als[kk - 1];
        F_1A734(*out, pp8->id, p12->b10 + pp8->dx, p12->b11 + pp8->dy, 0);
    }
    if (q) {
        bm = F_15BBE(m_8272.m8280, 2);
        F_1A472(bm, *out, p12->b2, p12->b3 - bm->height + row8 + 1);
        F_19438(1);
    }
    j5 = 1;
    for (j5 = 1; j5 < n; j5++)
        out[j5] = F_1934E((*out)->width + (r_vid.mode == -6 ? 0 : 8), (*out)->height,
            r_vid.mode != -6, 1);
    if (n > 1) {
        switch (r_vid.mode) {
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
            switch (r_vid.mode) {
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
            switch (r_vid.mode) {
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

/* F_181C8 @ 0F38:8E48 (linear 0x181C8, 2323 bytes)

   Draw a pop-up picture (car part, kind 1..8) with its frame sprite, saving and restoring the
   screen behind it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void F_181C8(int kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids)
{
    int n2;
    struct Blt blt;
    struct Bitmap far *bms6[4];
    int ang2;
    int w1;
    struct Bitmap far *scratch;
    int hh8;
    struct Sprite *spr3;
    int x0;
    struct Rect *pr16;
    int w2;
    int x2;
    struct Bitmap far *pb8;
    int top7;
    struct Rect *pr26;
    int h2;
    struct Bitmap far *bm48;
    int right;
    int tmpn;
    int h7;
    char far *mask3;
    int x7;
    struct Bitmap far *pan;

    top7 = bot - 0x45;
    F_120FE(-4);
again:
    n2 = (kind == 3 || kind == 7 || kind == 8) ? 1 : 4;
    F_15B7F(g7d86n[idx].f8, &w1, &h7, &x7);
    g_02d0 = g_02a6[g_0284];
    tmpn = (r_vid.mode != -2 && r_vid.mode != -6) ? g_02b4[g_0284] : g_02c2[g_0284];
    hh8 = (kind == 3 || kind == 7 || kind == 8) ? h7 - g_02d0 : g_0298[g_0284];
    x0 = ((0x140 - w1) / 2 + 4) & 0xfff8;
    right = kind == 2 ? 0x140 - x0 : x0 + w1;
    if (g_5a24 != 2) {
        if (kind == 4) {
            g_4f4a = 1;
            if (r_vid.mode == -2) {
                F_03904(g_5a0a, bp7316[2]->bits.p, 0, 0x140, 0x46 - g_02d0, -2);
                F_24E78(r_vid.back->bits.p, g_5a0a->bits.p - 0x28 * top7, 0x28, g_5a0a->height, 0, top7, 0x28);
            } else {
                F_03904(g_5a0a, bp7316[2]->bits.p, 0, 0x140, 0x46 - g_02d0, -1);
                F_11A72(0, top7, 0x140, g_5a0a->height, g_5a0a);
            }
        }
        if (kind == 4) {
            F_193D4();
            if (g_0284 == 3) {
                F_15B7F(0x449, &w2, &h2, &x2);
                g_028c = F_1934E(w2, h2, 0, 0);
                g_0290 = F_1934E(w2, h2, 0, 0);
            }
            if (g_0284 == 2 || g_0284 == 6)
                F_191DD();
        } else
            F_193FE();
        if (!g_0294)
            g_0294 = F_1934E(0x40, 0x19, 0, 0);
        F_17BE6(bms6, idx, flags, kk, q, ids, 1, n2);
        if (r_vid.mode == -6 && kind != 3 && kind != 7 && kind != 8) {
            F_176B6(bms6, n2, g_02d0, hh8, tmpn);
            bms6[1] = bms6[2] = bms6[3] = bms6[0];
            F_1941E(3);
        }
        if (kind != 3 && kind != 7 && kind != 8) {
            if (g_5a24 != 1) {
                g_5a22 = right;
                g_5a20 = kind == 2 ? x0 : 0;
            } else {
                g_5a22 = 0x140;
                g_5a20 = 0;
            }
            F_1783E(g_5a0a, &pan, mode2 ? 0x448 : 0, &pb8, 0x46, h7, &pr16, &pr26);
        }
    }
    ac825et = x0;
    aw8260y = bot - bms6[0]->height + 1;
    l_8262 = bms6[0]->height - g_02d0;
    if (d_8266 > 0) {
        d_8266 += x0;
        ap8268t += aw8260y;
        g826af += x0;
        ae826cv += aw8260y;
    }
    ar826e += x0;
    q_8270 += aw8260y;
    m_8272.m8272 += x0;
    m_8272.m8274 += aw8260y;
    ang2 = (kind == 1 || kind == 4) ? x0 - right : x0;
    if (g_5a24 != 2) {
        if (r_vid.mode == -2) {
            if (kind == 3 || kind == 8)
                mask3 = g_5a0a->bits.p + ((g_5a0a->height - hh8) * g_5a0a->pitch + x0 / 8);
            else
                mask3 = 0;
            F_1751A(dz7ec2, bms6, n2, g_02d0, hh8, mask3, tmpn);
        }
        if (!((r_vid.mode == -2 || r_vid.mode == -6) && r_vid.mode != -6)) {
            if (kind == 3 || kind == 7 || kind == 8) {
                bm48 = F_1934E(w1, h7 - g_02d0, 0, 1);
                if (kind != 7) {
                    blt.w = w1;
                    blt.h = h7 - g_02d0;
                    blt.sx = x0;
                    blt.sy = g_5a0a->height - h7;
                    blt.dx = blt.dy = 0;
                    (*b_78a2)(g_5a0a, bm48, &blt, 1);
                } else
                    F_11A72(x0, aw8260y, bm48->width, bm48->height, bm48);
                blt.w = w1;
                blt.h = blt.fc = h7 - g_02d0;
                blt.dx = blt.dy = blt.sx = blt.sy = 0;
                (*ab78aat)(bms6[0], bm48, &blt, 1);
            } else if (r_vid.mode != -6)
                F_176B6(bms6, n2, g_02d0, hh8, tmpn);
        }
        if (!((r_vid.mode == -2 || r_vid.mode == -6) && r_vid.mode != -6)) {
            blt.w = w1;
            blt.h = h7 - g_02d0;
            blt.sx = blt.sy = 0;
            blt.dx = x0;
            blt.dy = bot - h7 + 1;
        }
        if (kind == 3) {
            if (r_vid.mode == -2)
                F_24EFB(dz7ec2[0].p, bms6[0]->pitch, bms6[0]->height - g_02d0,
                    r_vid.back->bits.p + ((bot - h7) * 0x28 + (x0 >> 3)) + 0x28, bms6[0]->pitch);
            else
                (*b_78a2)(bm48, r_vid.back, &blt, 1);
            (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
            goto done;
        } else if (kind == 7 || kind == 8) {
            if (r_vid.mode == -2)
                F_24EFB(dz7ec2[0].p, bms6[0]->pitch, h7 - g_02d0,
                    r_vid.front->bits.p + ((bot - h7) * 0x28 + (x0 >> 3)) + 0x28, bms6[0]->pitch);
            else
                (*b_78a2)(bm48, r_vid.front, &blt, 1);
            F_1A92A(r_vid.front, r_vid.back, x0, bot - h7 + 1, x0 + w1 - 1, bot);
            goto done;
        }
    }
    spr3 = F_195F3(ang2, bot, bms6, (char (*)[8])dz7ec2, n2, pb8, pan, pr16, pr26);
    F_19B6E(spr3, kind, right);
    if (kind == 6)
        spr3->page->r = g_6cbc;
    if (kind == 4 || kind == 1 || kind == 2 || kind == 6) {
        g_0286 = 1;
        g_5a0e = 1;
        if (g_5a24 != 2)
            F_01AAD();
        while ((*g78c6c)(spr3))
            ;
        if (g_5a24 != 1)
            F_01B27();
        bu825a = g_0288;
        g_5a0e = 0;
        g_0286 = 0;
        g_6cbc = spr3->page->r;
    }
    if (g_0284 == 3 && kind == 4) {
        scratch = F_1934E(0x18, 7, 0, 2);
        blt.w = 0x10;
        blt.h = scratch->height;
        blt.sx = ar826e - ac825et;
        blt.sy = q_8270 - aw8260y;
        blt.dx = blt.dy = 0;
        (*b_78a2)(bms6[0], scratch, &blt, 1);
        F_1A734(bms6[0], 0x472, (char)blt.sx, (char)blt.sy, 0);
        F_17AAE(0x40, 0x64, z_8b7c, bot, g_028c, 0x4c8, 0x4ca);
        F_17AAE(0x40, 0x64, z_8b7c, bot, g_0290, 0x449, 0x44a);
        blt.dx = blt.sx;
        blt.dy = blt.sy;
        blt.sx = blt.sy = 0;
        (*b_78a2)(scratch, bms6[0], &blt, 1);
    }
    F_1013D(2);
    if (r_vid.mode != -2)
        (*dz78b6)(r_vid.front->bits.p, r_vid.back->bits.p);
    if (g_5a24 == 1) {
        kind = 2;
        g_5a24 = 2;
        goto again;
    }
done:
    g_02d0 = g_5a22 = g_5a20 = 0;
    F_193FE();
    g_5a24 = 0;
    F_120FE(-2);
}

/* F_18ADB @ 0F38:975B (linear 0x18ADB, 256 bytes)

   Blit a clipped band of a bitmap through the blit hook, then the seven frame cells onto the front
   buffer */

void F_18ADB(struct Bitmap far *dst, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    int base7, hi1, lo7, i9;
    struct Blt r;
    int top;

    base7 = p7 - 0x45;
    top = g_5a0a->height + base7 - 1;
    lo7 = p6 < base7 ? base7 : p6;
    hi1 = p6 + p4 - 1 > top ? top : p6 + p4 - 1;
    if (lo7 <= hi1) {
        r.w = p3;
        r.h = hi1 - lo7 + 1;
        r.sx = p1;
        r.sy = p2 + lo7 - p6;
        r.dx = p5;
        r.dy = lo7 - base7;
        (*b_78a2)(dst, g_5a0a, &r, 1);
    }
    if (g_0284 == 6) {
        r.h = g_5a0a->height;
        r.sy = p7 - 0x45;
        r.dy = 0;
        for (i9 = 0; i9 < 7; i9++) {
            r.w = g_5338[i9].w;
            r.sx = r.dx = g_5338[i9].x;
            (*b_78a2)(r_vid.front, g_5a0a, &r, 1);
        }
    }
}
