/* F_21B7C: for the four interleaved banks work out the byte count and start offset of each */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_21B7C(int x, int x0, int y, int *b, int *a)
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
