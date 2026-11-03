/* F_065AF @ 0000:65AF (linear 0x065AF, 417 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0653C(int v, int *out);

void F_065AF(int a1)
{
    int x;
    struct Bitmap far *pic;
    int py4, px5, row5, m, id, d9, al, y4;
    struct Bitmap far *img;
    int e9, col, day, bl;

    F_06529(0);
    if (a1 == 0 && g_0282)
        return;
    d9 = F_06476();
    F_064A3(&al, &bl);
    m = F_0653C(d9, &day);
    col = (e9 = d9 + 21) % 7;
    row5 = e9 / 7 - g_5052[m];
    x = 18 * col + g_5048;
    y4 = 7 * row5 + g_504a[m];
    g_vid.f16 = 1;
    px5 = 80;
    py4 = 4;
    id = m < 2 ? 0x4c3 : 0x4c5;
    F_120FE(-3);
    pic = F_15BBE(id, 2);
    F_1A734(pic, 0x4c4, (unsigned char)x, (unsigned char)y4, 0);
    img = F_1934E(pic->width, pic->height, 0, 2);
    F_11A72(px5, py4, pic->width, pic->height, img);
    F_11BD3(pic, px5, py4);
    F_19438(1);
    F_120FE(-1);
    if (a1 == 0) {
        F_11421(-1, "", 0);
        F_03A83(500);
        g_05fc += 0L;
        F_11BD3(img, px5, py4);
    }
    F_19438(1);
}
