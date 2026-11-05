#include <conio.h>
#include "EGASLIDE.H"
#include "BITMAP.H"
#include "CRTC.H"
#include "DASH.H"
#include "EGABLIT.H"
#include "EGALATCH.H"
#include "GARGFX.H"
#include "MAIN.H"
#include "MASKBLT.H"
#include "MASKOR.H"
#include "PC.H"

/* Copy a range of rows of a planar sprite sheet into the back page, plane by plane */

void ev_sprite(int top, int bottom, int x, int y0, int y1, struct Bitmap far *bm)
{
    int k5;
    int pitch; /* bm pitch */
    int stridem;
    union FarPtr src0;
    union FarPtr alt16;
    char far *dst2k;
    union FarPtr curn;
    int stepl;
    int hh8;
    int psizem; /* bm plane size */
    char far *row;
    char far *dstm;
    char far *scratch;
    int y;
    int i;
    int rowsn;  /* bm height */
    int bpitch; /* r_vid.back pitch */
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
    mk_make_mask(bm->bits.p, bm->alt.p, bm->planesize);
    abase = bm->alt;
    eb_latch_pack(dst2k, pitch, bottom - top + 1, scratch, bpitch);
    for (i = 0, y = y0; i < n; i++, y += stepl, row += stridem) {
        ea_to_page(scratch, pitch, bottom - top + 1, dst2k, pitch);
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
            outp(SEQ_INDEX, SEQ_MAP_MASK);
            outp(SEQ_DATA, k5);
            outp(GC_INDEX, GC_READ_MAP);
            outp(GC_DATA, m);
            mr_blit(dstm, curn.w.seg, curn.w.off, alt16.w.off, (char)pitch, (char)pitch, (char)hh8, (char)bpitch);
        }
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, SEQ_ALL_PLANES);
        if (r_vid.copyToFront) {
            if (g_boltOverlay)
                (*g_videoOps.maskedBlit)(g_boltOverlay, r_vid.back, &g_cpyBoltPatchRecord, 1);
            ct_wait_vsync();
            ea_rect(r_vid.back->bits.p, r_vid.front->bits.p, pitch, bottom - top + 1, x, top, bpitch);
            ct_wait_vsync();
        }
    }
}
