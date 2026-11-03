/* F_2B857 @ 2645:5407 (linear 0x2B857, 497 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2B6DC(unsigned char b);
static struct Bitmap far * near F_2B707(struct Bitmap far *p, unsigned char a, unsigned char b);

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
        F_26401(q->bits.p, q->alt.p, q->planesize);
    return q;
}
