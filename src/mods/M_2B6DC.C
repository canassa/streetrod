#include "STRUCTS.H"
#include "M_2B6DC.H"
#include "ARENA.H"
#include "MAIN.H"
#include "MASKBLT.H"
#include "M_0F38A.H"
#include "TANDYPIC.H"

static int near F_2B6DC(unsigned char b);
static struct Bitmap far * near F_2B707(struct Bitmap far *p, unsigned char a, unsigned char b);
static void near F_2BA48(unsigned char v, char *pa, char *pb);

/* F_2B6DC @ 2645:528C (linear 0x2B6DC, 43 bytes)

   Counts the set bits of a byte. */

static int near F_2B6DC(unsigned char b)
{
    int count;
    int bit;

    count = 0;
    for (bit = 0; bit < 8; bit++) {
        if (b & 0x80)
            count++;
        b <<= 1;
    }
    return count;
}

/* F_2B707 @ 2645:52B7 (linear 0x2B707, 336 bytes) */

static struct Bitmap far * near F_2B707(struct Bitmap far *p, unsigned char a, unsigned char b)
{
    int nb1;
    int rows6;
    struct Bitmap far *q;
    int t;
    int bytes;
    int m2;
    int remk;
    int na;

    na = F_2B6DC(a);
    nb1 = F_2B6DC(b);
    bytes = ((p->width & ~7) * nb1) >> 3;
    remk = p->width & 7;
    for (t = 0, m2 = 0x80; t < remk; t++) {
        if (b & m2)
            bytes++;
        m2 >>= 1;
    }
    bytes = (bytes + ~r_vid.mask) & r_vid.mask;
    rows6 = ((p->height & ~7) * na) >> 3;
    remk = p->height & 7;
    for (t = 0, m2 = 0x80; t < remk; t++) {
        if (a & m2)
            rows6++;
        m2 >>= 1;
    }
    q = F_1934E(bytes, rows6, p->alt.p != 0, 1);
    F_109C0(q->bits.p, 0, q->planesize);
    F_109C0(q->alt.p, 0xff, q->planesize / (unsigned)r_vid.planes);
    return q;
}

/* F_2B857 @ 2645:5407 (linear 0x2B857, 497 bytes) */

struct Bitmap far *F_2B857(struct Bitmap far *p, unsigned char a, unsigned char b)
{
    unsigned char far *dpl;
    struct Bitmap far *q;
    unsigned char far *sp;
    unsigned char out;
    int spitch;
    int qpitch4;
    unsigned char m;
    int x6, y5, planen, bitoff4;
    unsigned char bt8;
    int nbn, qh6;
    unsigned char k4, bbk, tk;
    int bit, sh4, na;

    na = F_2B6DC(a);
    nbn = F_2B6DC(b);
    q = F_2B707(p, a, b);
    sp = p->bits.p;
    sh4 = p->height;
    spitch = p->pitch;
    dpl = q->bits.p;
    qpitch4 = q->pitch;
    qh6 = q->height;
    for (planen = 0; planen < 4; planen++) {
        dpl = q->bits.p + planen * qh6 * qpitch4;
        m = 0x80;
        for (y5 = 0; y5 < sh4; y5++) {
            if (a & m) {
                bitoff4 = 0;
                for (x6 = 0; x6 < spitch; x6++) {
                    bt8 = *sp;
                    out = 0;
                    bbk = b;
                    k4 = 0x80;
                    for (bit = 0; bit < 8; bit++) {
                        if (bbk & k4) {
                            out |= bt8 & k4;
                            k4 >>= 1;
                        } else {
                            bt8 <<= 1;
                            bbk <<= 1;
                        }
                    }
                    tk = out >> bitoff4;
                    *dpl |= tk;
                    bitoff4 += nbn;
                    if (bitoff4 > 8) {
                        dpl++;
                        tk = out << (nbn - bitoff4 + 8);
                        *dpl = tk;
                        bitoff4 &= 7;
                    }
                    if (bitoff4 == 8) {
                        dpl++;
                        bitoff4 = 0;
                    }
                    sp++;
                }
                if (bitoff4)
                    dpl++;
            } else
                sp += spitch;
            m >>= 1;
            if (m == 0)
                m = 0x80;
        }
    }
    if (p->alt.p)
        maskblt_make_mask(q->bits.p, q->alt.p, q->planesize);
    return q;
}

/* F_2BA48 @ 2645:55F8 (linear 0x2BA48, 120 bytes) */

static void near F_2BA48(unsigned char v, char *pa, char *pb)
{
    int m;
    int sh;

    *pa = *pb = 0;
    for (m = 0x80, sh = 6; m > 8; m >>= 1, sh -= 2)
        if (v & m)
            *pa |= 3 << sh;
    for (m = 8, sh = 6; m > 0; m >>= 1, sh -= 2)
        if (v & m)
            *pb |= 3 << sh;
}

/* F_2BAC0 @ 2645:5670 (linear 0x2BAC0, 533 bytes) */

struct Bitmap far *F_2BAC0(struct Bitmap far *p, unsigned char a, unsigned char c)
{
    unsigned char far *dpl;
    struct Bitmap far *q;
    unsigned char ab, mm27, hh3;
    char pb1;
    unsigned char far *sp;
    unsigned char out;
    int shn;
    char pa8;
    unsigned char far *ap8;
    unsigned char far *ap26;
    int x1, y;
    int cnt4;
    unsigned char bt3;
    unsigned char ml;
    int spitchn;
    unsigned char mm, v4;
    int j;

    q = F_2B707(p, a, c);
    sp = p->bits.p;
    ap8 = p->alt.p;
    shn = p->height;
    spitchn = p->pitch;
    dpl = q->bits.p;
    ap26 = q->alt.p;
    ml = 0x80;
    F_2BA48(c, &pa8, &pb1);
    for (y = 0; y < shn; y++) {
        if (a & ml) {
            cnt4 = out = 0;
            mm27 = 0xc0;
            hh3 = 0xff;
            for (x1 = 0; x1 < spitchn; x1++, sp++, ap8++) {
                bt3 = *sp;
                ab = *ap8;
                v4 = (x1 & 1) ? pb1 : pa8;
                for (j = 0, mm = 0xc0; j < 4; j++, mm >>= 2) {
                    if (v4 & mm) {
                        if (cnt4 < j)
                            out |= (bt3 & mm) << ((j - cnt4) * 2);
                        else
                            out |= (bt3 & mm) >> ((cnt4 - j) * 2);
                        if (!(ab & mm))
                            hh3 &= ~mm27;
                        mm27 >>= 2;
                        cnt4++;
                        if (cnt4 == 4) {
                            mm27 = 0xc0;
                            *dpl++ = out;
                            *ap26++ = hh3;
                            cnt4 = out = 0;
                            hh3 = 0xff;
                        }
                    }
                }
            }
            if (cnt4) {
                *dpl = out;
                dpl++;
                *ap26 = hh3;
                ap26++;
            }
        } else {
            sp += spitchn;
            ap8 += spitchn;
        }
        ml >>= 1;
        if (ml == 0)
            ml = 0x80;
    }
    return q;
}

/* F_2BCD5 @ 2645:5885 (linear 0x2BCD5, 470 bytes) */

struct Bitmap far *F_2BCD5(struct Bitmap far *p, unsigned char a, unsigned char c)
{
    unsigned char far *dpl;
    struct Bitmap far *q;
    unsigned char far *sp;
    unsigned char out;
    unsigned char far *row1;
    int x1, y, phase8, qpitch;
    unsigned char ml;
    int spitchn, sh;
    unsigned char mm2;

    q = F_2B707(p, a, c);
    sp = p->bits.p;
    sh = p->height;
    spitchn = p->pitch;
    row1 = q->bits.p;
    qpitch = q->pitch;
    ml = 0x80;
    for (y = 0; y < sh; y++) {
        if (a & ml) {
            dpl = row1;
            row1 += qpitch;
            phase8 = out = 0;
            mm2 = 0x80;
            for (x1 = 0; x1 < spitchn; x1++, sp++) {
                if (c & mm2) {
                    out |= (*sp & 0xf0) >> (phase8 ? 4 : 0);
                    if (phase8) {
                        *dpl++ = out;
                        phase8 = out = 0;
                    } else
                        phase8 = 1;
                }
                mm2 >>= 1;
                if (c & mm2) {
                    out |= (*sp & 0x0f) << (phase8 ? 0 : 4);
                    if (phase8) {
                        *dpl++ = out;
                        phase8 = out = 0;
                    } else
                        phase8 = 1;
                }
                mm2 >>= 1;
                if (mm2 == 0)
                    mm2 = 0x80;
            }
            if (phase8)
                *dpl = out;
        } else
            sp += spitchn;
        ml >>= 1;
        if (ml == 0)
            ml = 0x80;
    }
    tandypic_make_mask(q->bits.p, q->alt.p, q->planesize);
    return q;
}
