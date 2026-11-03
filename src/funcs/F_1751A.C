/* F_1751A: for each of n bitmaps, lay a band of h rows into the scratch bitmap g_731e, fill or copy it, then blit the bitmap through the EGA planes */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

static void near F_174A6(char far *src, char far *dst, char far *mask, int w, int h, int n, int step);

static void near F_1751A(struct Band *A, struct Bitmap far **B, int n, int y2, int h,
    char far *mask, char c)
{
    int i;
    int j;
    char far *base;

    base = g_731e->bits.p + 0x46 * g_731e->pitch;
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
