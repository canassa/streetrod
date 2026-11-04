#include <dos.h>
#include <memory.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "TANDYBLT.H"
#include "CGADRV.H"
#include "DASH.H"
#include "MASKBG.H"
#include "MASKBLT.H"
#include "MASKOR.H"
#include "TANDYCPY.H"

static void near split_banks(int x, int x0, int y, int *b, int *a);
static void near copy_from_tandy(struct Bitmap far *src, struct Bitmap far *dst, struct Clip *o);
static void near shift_nibbles(unsigned char *p, int dir, int n);
static void near put_nibbles(char far *dst, char *src, int n, int odd);

/* Copy rows from a four-bank interleaved bitmap to a linear bitmap, one row per movedata */

static void near copy_from_tandy(struct Bitmap far *src, struct Bitmap far *dst, struct Clip *blt)
{
    char far *basel;
    int end, pitch, x, w2, i, y;
    unsigned sg1[4];
    char far *s8;
    int off;

    x = blt->sx;
    y = blt->sy;
    end = blt->h + y;
    s8 = dst->bits.p + ((pitch = dst->pitch) * blt->dy + blt->dx);
    w2 = blt->w;
    basel = src->bits.p;
    off = (y >> 2) * 160;
    off += x;
    off += FP_OFF(basel);
    sg1[0] = FP_SEG(basel);
    sg1[1] = sg1[0] + 0x200;
    sg1[2] = sg1[1] + 0x200;
    sg1[3] = sg1[2] + 0x200;
    for (i = y; i < end; i++, s8 += pitch) {
        movedata(sg1[i & 3], off, FP_SEG(s8), FP_OFF(s8), w2);
        if ((i & 3) == 3)
            off += 160;
    }
}

/* For the four interleaved banks work out the byte count and start offset of each */

static void near split_banks(int x, int x0, int y, int *b, int *a)
{
    unsigned d;
    int e;
    int c;
    unsigned f;

    c = (y >> 2) * 160 + x0;
    d = y & 3;
    a[0] = a[1] = a[2] = a[3] = x >> 2;
    e = 0;
    f = (unsigned char)x & 3;
    for (; e < 4; e++) {
        if (e < f)
            a[e]++;
        b[e] = (d << 13) + c;
        if (++d == 4) {
            d = 0;
            c += 160;
        }
    }
}

/* Masked blit between interleaved-bank bitmaps, one mg_blit call per bank */

void tb_compose(struct Bitmap far *dst, struct Bitmap far *src, struct Bitmap far *mask, struct Blt *in)
{
    int seg2;
    int a[4];
    int k6;
    union FarPtr alt_a;
    struct Clip clip9;
    int p1;
    int p2o;
    int pcn;
    union FarPtr bits_cm;
    int pa;
    unsigned off6;
    int wz;
    union FarPtr bits_bn;
    union FarPtr bits_a;
    int b[4];
    unsigned di;

    bits_a = dst->bits;
    alt_a = dst->alt;
    bits_bn = src->bits;
    bits_cm = mask->bits;
    if (!da_clip_blt(dst, src, in, (struct Blt *)&clip9))
        return;
    pa = dst->pitch;
    wz = clip9.w;
    pcn = mask->pitch;
    seg2 = bits_a.w.seg;
    off6 = clip9.sy * pa + clip9.sx;
    p1 = bits_a.w.off + off6;
    p2o = alt_a.w.off + off6;
    di = (in->my * pcn + (in->mx >> 1)) + bits_cm.w.off;
    split_banks(clip9.h, clip9.dx, clip9.dy, b, a);
    for (k6 = 0; k6 < 4; k6++) {
        mg_blit(bits_bn.p + b[k6], seg2, p1, p2o, di, pa * 4, wz, a[k6], pcn * 4, 0xa0);
        p1 += pa;
        p2o += pa;
        di += pcn;
    }
}

/* Blit between bitmaps through the blitter selected by flag, in banks when the destination is
   interleaved */

void tb_masked(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in, int flag)
{
    union FarPtr bits_b;
    int pa6;
    int w;
    void(far * fn4)(char far *, unsigned, int, int, int, int, int, int);
    struct Blt clip;
    int h;
    int pb9;
    int k;
    int off2;
    int al[4];
    int bl[4];
    int off3;
    int off04;
    int dy2;
    int seg;

    if (!da_clip_blt(src, dst, in, &clip))
        return;
    pa6 = src->pitch;
    w = clip.w;
    h = clip.h;
    dy2 = clip.dy;
    pb9 = dst->pitch;
    bits_b = dst->bits;
    seg = src->bits.w.seg;
    off3 = (off04 = clip.sy * pa6 + clip.sx) + src->bits.w.off;
    off2 = src->alt.w.off + off04;
    fn4 = flag ? mr_blit : mk_copy;
    if (dst->flags >= -1) {
        (*fn4)(bits_b.p + pb9 * dy2 + clip.dx, seg, off3, off2, pa6, w, h, pb9);
    } else {
        split_banks(clip.h, clip.dx, dy2, bl, al);
        for (k = 0; k < 4; k++) {
            (*fn4)(bits_b.p + bl[k], seg, off3, off2, pa6 * 4, w, al[k], 0xa0);
            off3 += pa6;
            off2 += pa6;
        }
    }
}

/* Clipped blit between bitmaps, picking the plain, interleaved or planar copy routine */

void tb_copy(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *r, int flags)
{
    int sp3;
    int f;
    int dp3;
    int skip;
    int dskip;
    int g8;
    struct Clip o;

    f = flags & 1;
    g8 = (flags & 2) && src->alt.p && dst->alt.p;
    if (!da_clip_blt(src, dst, r, (struct Blt *)&o))
        return;
    if (src->flags < -1 && dst->flags < -1)
        ty_screen(src->bits.p, dst->bits.p, o.w, o.h, o.sx, o.sy, o.dx, o.dy);
    else if (src->flags < -1)
        copy_from_tandy(src, dst, &o);
    else {
        sp3 = src->pitch * o.sy + o.sx;
        skip = src->pitch - o.w;
        dskip = dst->pitch - o.w;
        if (dst->flags >= -1) {
            dp3 = dst->pitch * o.dy + o.dx;
            if (f)
                cd_blit_rect(src->bits.p + sp3, dst->bits.p + dp3, o.w, o.h, skip, dskip);
            if (g8)
                cd_blit_rect(src->alt.p + sp3, dst->alt.p + dp3, o.w, o.h, skip, dskip);
        } else {
            dp3 = dst->pitch * (o.dy >> 2) + ((o.dy & 3) << 13) + o.dx;
            ty_to_screen(src->bits.p + sp3, dst->bits.p + dp3, o.w, o.h, skip, dskip, o.dy);
        }
    }
}

/* Shift a run of packed nibbles by one nibble, forwards or backwards */

static void near shift_nibbles(unsigned char *p, int dir, int n)
{
    unsigned char carry;
    unsigned char c;
    unsigned rot;
    int i;

    if (dir > 0) {
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotr(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p++ = (c & 0x0f) | carry;
            carry = c & 0xf0;
        }
    } else if (dir < 0) {
        p += n - 1;
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotl(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p-- = (c & 0xf0) | carry;
            carry = c & 0x0f;
        }
    }
}

/* Copy n packed nibbles to a far buffer, starting at the low nibble when odd */

static void near put_nibbles(char far *dst, char *src, int n, int odd)
{
    unsigned char *s;

    s = src;
    if (odd) {
        *dst &= 0xf0;
        *dst |= *s++ & 0x0f;
        dst++;
        n--;
    }
    for (; n > 1; n -= 2)
        *dst++ = *s++;
    if (n > 0) {
        *dst &= 0x0f;
        *dst |= *s & 0xf0;
    }
}

/* Copy rows between 2-pixel-per-byte bitmaps, shifting each row through a buffer by a nibble */

void tb_shifted(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt)
{
    int n2;
    int w9;
    char far *dn[4];
    unsigned char buf6[160];
    char far *c;
    unsigned char far *s;
    int h1;
    int i;
    int sh;
    int sx;
    int off;
    int dx;
    int k1;
    int ph4;
    int sy;
    int off2;
    int dy;
    sx = blt->sx;
    sy = blt->sy;
    w9 = blt->w;
    h1 = blt->h;
    dx = blt->dx;
    dy = blt->dy;
    s = src->bits.p + src->pitch * sy + (sx >> 1);
    dn[0] = dst->bits.p;
    dn[1] = dn[0] + (dst->planesize >> 2);
    dn[2] = dn[1] + (dst->planesize >> 2);
    dn[3] = dn[2] + (dst->planesize >> 2);
    off2 = dst->pitch * (dy >> 2) + (dx >> 1);
    n2 = ((sx + w9 - 1) >> 1) - (sx >> 1) + 1;
    ph4 = dx & 1;
    sh = ph4 - (sx & 1);
    for (i = 0; i < h1; i++, s += src->pitch) {
        for (off = 0; off < n2; off++)
            buf6[off] = s[off];
        k1 = (i + dy) & 3;
        c = dn[k1] + off2;
        if (k1 == 3)
            off2 += dst->pitch;
        if (sh)
            shift_nibbles(buf6, sh, n2);
        put_nibbles(c, buf6, w9, ph4);
    }
}
