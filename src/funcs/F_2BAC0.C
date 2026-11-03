/* F_2BAC0 @ 2645:5670 (linear 0x2BAC0, 533 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Bitmap far * near F_2B707(struct Bitmap far *p, unsigned char a, unsigned char b);
static void near F_2BA48(unsigned char v, char *pa, char *pb);

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
