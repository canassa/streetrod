/* F_2BCD5 @ 2645:5885 (linear 0x2BCD5, 470 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Bitmap far * near F_2B707(struct Bitmap far *p, unsigned char a, unsigned char b);

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
    F_24849(q->bits.p, q->alt.p, q->planesize);
    return q;
}
