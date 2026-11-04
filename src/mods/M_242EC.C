#include <conio.h>
#include "STRUCTS.H"
#include "M_242EC.H"
#include "M_00000.H"
#include "M_10B7C.H"
#include "M_123F4.H"
#include "M_21A06.H"
#include "M_23262.H"
#include "M_24E78.H"
#include "M_25954.H"
#include "M_2634A.H"

#pragma intrinsic(outp)

/* F_242EC @ 21A0:28EC: copy a range of rows of a planar sprite sheet into the back page, plane by plane */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void F_242EC(int top, int bottom, int x, int y0, int y1, struct Bitmap far *bm)
{
    int k5;
    int pitch;            /* bm pitch */
    int stridem;
    union FarPtr src0;
    union FarPtr alt16;
    char far *dst2k;
    union FarPtr curn;
    int stepl;
    int hh8;
    int psizem;           /* bm plane size */
    char far *row;
    char far *dstm;
    char far *scratch;
    int y;
    int i;
    int rowsn;            /* bm height */
    int bpitch;           /* r_vid.back pitch */
    union FarPtr base1;
    union FarPtr abase;
    int m;
    int n;

    x /= 8;
    pitch = bm->pitch;
    rowsn = bm->height;
    psizem = pitch * rowsn;
    bpitch = r_vid.back->pitch;
    scratch = r_vid.back->bits.p + 0x2af0;
    dst2k = r_vid.back->bits.p + top * bpitch + x;
    n = y1 - y0;
    base1 = bm->bits;
    if (n < 0) {
        if (n & 1) {
            n--;
            y0++;
        }
        stridem = -(bpitch * 2);
        stepl = -2;
        n = -n;
    } else {
        if (n & 1) {
            n++;
            y0--;
        }
        stridem = bpitch * 2;
        stepl = 2;
    }
    n = (n >> 1) + 1;
    row = r_vid.back->bits.p + y0 * bpitch + x;
    F_26401(bm->bits.p, bm->alt.p, bm->planesize);
    abase = bm->alt;
    F_2382D(dst2k, pitch, bottom - top + 1, scratch, bpitch);
    for (i = 0, y = y0; i < n; i++, y += stepl, row += stridem) {
        F_24EFB(scratch, pitch, bottom - top + 1, dst2k, pitch);
        if (y < top) {
            src0.p = base1.p + (top - y) * pitch;
            dstm = dst2k;
            alt16.p = abase.p + (top - y) * pitch;
            hh8 = rowsn - top + y;
        } else {
            dstm = row;
            src0 = base1;
            alt16 = abase;
            hh8 = (y + rowsn - 1 <= bottom) ? rowsn : bottom - y + 1;
        }
        k5 = 1;
        m = 0;
        curn = src0;
        for (; k5 < 9; k5 <<= 1, m++, curn.w.off += psizem) {
            outp(0x3c4, 2);
            outp(0x3c5, k5);
            outp(0x3ce, 4);
            outp(0x3cf, m);
            F_25954(dstm, curn.w.seg, curn.w.off, alt16.w.off, (char)pitch, (char)pitch, (char)hh8, (char)bpitch);
        }
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
        if (r_vid.f16) {
            if (g_0792)
                (*ab78aat)(g_0792, r_vid.back, &u_70ac, 1);
            F_21A06();
            F_24E78(r_vid.back->bits.p, r_vid.front->bits.p, pitch, bottom - top + 1, x, top, bpitch);
            F_21A06();
        }
    }
}
