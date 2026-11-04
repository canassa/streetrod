#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdio.h>

static int near F_0653C(int v, int *out);

/* F_0653C @ 0000:653C (linear 0x0653C, 115 bytes) */


static int near F_0653C(int v, int *out)
{
    int r;

    if (v < 15) {
        r = 0;
        *out = v + 16;
    } else if (v < 46) {
        r = 1;
        *out = v - 14;
    } else if (v < 77) {
        r = 2;
        *out = v - 45;
    } else if (v < 91) {
        r = 3;
        *out = v - 76;
    } else {
        r = 3;
        *out = 15;
    }
    return r;
}

/* F_065AF @ 0000:65AF (linear 0x065AF, 417 bytes) */



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
    r_vid.f16 = 1;
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

char *g_5082[4] = {"June", "July", "August", "September"};   /* DS:5082 */
int g_508a[5] = {11, 13, 9, 10, 12};   /* DS:508A */

/* F_06750 @ 0000:6750 (linear 0x06750, 127 bytes)  - draws the current date ("Month day, 1963") */



void F_06750(int x)
{
    int date;
    int today;
    int mnum;
    char buf[30];
    struct Colors sv;

    today = F_06476();
    mnum = F_0653C(today, &date);
    sprintf(buf, "%s %d, 1963", g_5082[mnum], date);
    sv = x_colors;
    x_colors = bustyle->hilite;
    (*a_78aem)(r_vid.back, 0x61, x, buf);
    x_colors = sv;
}

/* F_067CF @ 0000:67CF (linear 0x067CF, 88 bytes) */


void F_067CF(int a)
{
    if (r_vid.mode == -2 || r_vid.mode == -6) {
        if (gz8249) {
            F_112CB(6, g_50c2[a][0]);
            F_112CB(7, g_50c2[a][1]);
        }
        g_palette[6] = g_50c2[a][0];
        g_palette[7] = g_50c2[a][1];
    }
}
