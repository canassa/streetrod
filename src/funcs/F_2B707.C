/* F_2B707 @ 2645:52B7 (linear 0x2B707, 336 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2B6DC(unsigned char b);

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
    bytes = (bytes + ~g_vid.mask) & g_vid.mask;
    rows6 = ((p->height & ~7) * na) >> 3;
    remk = p->height & 7;
    for (t = 0, m2 = 0x80; t < remk; t++) {
        if (a & m2)
            rows6++;
        m2 >>= 1;
    }
    q = F_1934E(bytes, rows6, p->alt.p != 0, 1);
    F_109C0(q->bits.p, 0, q->planesize);
    F_109C0(q->alt.p, 0xff, q->planesize / (unsigned)g_vid.planes);
    return q;
}
