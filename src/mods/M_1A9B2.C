#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static struct Bitmap far * near F_1A9B2(int n);
static void near F_1ABAC(int a, int *p, int *q);

int g_5aa8 = 0;   /* DS:5AA8 */
int g_5aaa[3] = {1034, 1035, 1036};   /* DS:5AAA */
struct Bitmap far *g_5ab0 = (struct Bitmap far *)&g_6da4;   /* DS:5AB0 */
struct Bitmap far *g_5ab4 = (struct Bitmap far *)&g_6d40;   /* DS:5AB4 */
struct Bitmap far *g_5ab8 = (struct Bitmap far *)&g_6d70;   /* DS:5AB8 */

/* F_1A9B2: build the two scratch bitmaps for a shifted background strip and its 0x28-wide column */


static struct Bitmap far *near F_1A9B2(int n)
{
    struct Bitmap far *b;
    struct Blt blt;
    struct Bitmap far *a2;
    struct Bitmap far *c;

    a2 = F_1934E(g_6d30->width + 8, g_6d30->height, g_6d30->alt.p != 0L, 1);
    F_1A03F(g_6d30, n & ~g_vid.mask, a2);
    b = F_1934E(a2->width, a2->height, 0, 1);
    F_11A72((n + g_6da0) & g_vid.mask, 0x78, a2->width, a2->height, b);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_78aa)(a2, b, &blt, 1);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = 0;
    blt.dx = (n + g_6da0) & g_vid.mask;
    blt.dy = 0x78;
    (*g_78a2)(b, g_vid.back, &blt, 1);
    F_1941E(2);
    blt.w = 0x28;
    blt.h = g_6df6->height;
    blt.sx = n & g_vid.mask;
    blt.sy = blt.dx = 0;
    blt.dy = -(g_6df6->height + g_5aa8 - 0x95);
    c = F_1934E(0x28, 0x9c - g_5aa8, 1, 1);
    F_11A72(blt.sx + g_6da0, g_5aa8, 0x28, 0x9c - g_5aa8, c);
    F_109C0(c->alt.p, 0xff, c->planesize / g_vid.planes);
    (*g_78aa)(g_6df6, c, &blt, 1);
    (*g_78a2)(g_6df6, c, &blt, 2);
    return c;
}

/* F_1ABAC: build the four shifted copies of a background strip and animate the sprite over them */


static void near F_1ABAC(int a, int *p, int *q)
{
    int i;

    g_6d38 = F_15BBE(0x4d2, 1);
    g_6cf0 = F_15BBE(*p, 1);
    g_6d3e = a & ~g_vid.mask;
    g_6de2[0] = F_1934E(g_6cf0->width + 8, g_6cf0->height, 1, 1);
    F_1A03F(g_6cf0, g_6d3e, g_6de2[0]);
    g_6df2 = F_1934E(g_6de2[0]->width, g_6de2[0]->height, 1, 1);
    F_19ED0(g_6de2[0], g_6df2, 3);
    g_6ce6 = g_6de2[0]->height - g_6d38->height;
    F_1A472(g_6d38, g_6de2[0], g_6d3e, g_6ce6);
    if (g_vid.mode == -2)
        (*g_78b6)(g_7312[1]->bits.p, g_731a->bits.p);
    g_6ce8.x1 = 0;
    g_6ce8.y1 = 0;
    g_6ce8.x2 = g_6d2c->width - 1;
    g_6ce8.y2 = g_6d2c->height + g_5aa8 - 1;
    g_6d3c = F_195F3(a & g_vid.mask, 0x94, &g_6df2, 0, 1, g_5ab0, g_6d2c, &g_6ce8, 0);
    F_199DD(g_6d3c, 0, 8, 0, 1);
    while ((*g_78c6)(g_6d3c)) {
        if (g_6d3c->y == 0x7d)
            *g_6df2 = *g_6de2[0];
        F_10AFD(3);
    }
    if (g_vid.mode == -2)
        (*g_78b6)(g_7312[1]->bits.p, g_731a->bits.p);
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2 + g_6d3e) & ~g_vid.mask;
        if (g_6da2) {
            g_6cf0 = F_15BBE(p[i], 2);
            g_6de2[4 - i] = F_1934E(g_6cf0->width + 8, g_6cf0->height, 1, 1);
            F_1A03F(g_6cf0, g_6da2, g_6de2[4 - i]);
            F_19438(1);
        } else
            g_6de2[4 - i] = F_15BBE(p[i], 1);
        F_1A472(g_6d38, g_6de2[4 - i], g_6da2, g_6ce6);
    }
    i = 4;
    g_6d3c = F_195F3(a, 0x9b, g_6de2, 0, 4, g_5ab4, g_6d24, 0, 0);
    F_199DD(g_6d3c, 0, (a + g_6de2[0]->width) >> 1, -2, 0);
    F_168C8(1);
    while ((*g_78c6)(g_6d3c))
        F_10AFD(2);
    F_168C8(0);
    F_1941E(6);
    if (g_vid.mode == -2)
        (*g_78b6)(g_7312[1]->bits.p, g_731a->bits.p);
    g_6de2[0] = F_15BBE(*q, 1);
    g_6d3e = ((unsigned char)a & 7) >> 1;
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2) & ~g_vid.mask;
        if (g_6da2) {
            g_6cf0 = F_15BBE(q[i], 2);
            g_6de2[i] = F_1934E(g_6cf0->width + 8, g_6cf0->height, 1, 1);
            F_1A03F(g_6cf0, g_6da2, g_6de2[i]);
            F_19438(1);
        } else
            g_6de2[i] = F_15BBE(q[i], 1);
    }
    for (i = 0; i < 4; i++) {
        if (i == g_6d3e) {
            g_6df2 = F_1934E(g_6de2[i]->width, g_6de2[i]->height, 1, 1);
            F_19ED0(g_6de2[i], g_6df2, 3);
        }
        F_1A472(g_6d38, g_6de2[i], (i * 2) & ~g_vid.mask, g_6ce6);
    }
    i = 4;
    g_6d3c = F_195F3(-g_6de2[0]->width, 0x9b, g_6de2, 0, 4, g_5ab4, g_6d24, 0, 0);
    F_199DD(g_6d3c, 0, (a + g_6de2[0]->width >> 1) + 1, 2, 0);
    F_168C8(1);
    while ((*g_78c6)(g_6d3c))
        F_10AFD(2);
    F_168C8(0);
    if (g_vid.mode == -2)
        (*g_78b6)(g_7312[1]->bits.p, g_731a->bits.p);
    g_6ce8.x1 = 0;
    g_6ce8.y1 = 0;
    g_6ce8.x2 = g_6d2c->width - 1;
    g_6ce8.y2 = g_6d2c->height + g_5aa8 - 1;
    g_6d3c = F_195F3(a, 0x9b, &g_6de2[g_6d3e], 0, 1, g_5ab0, g_6d2c, &g_6ce8, 0);
    F_199DD(g_6d3c, 0, 8, 0, -1);
    while ((*g_78c6)(g_6d3c)) {
        if (g_6d3c->y == 0x7d)
            *g_6de2[g_6d3e] = *g_6df2;
        F_10AFD(3);
    }
    F_1941E(6);
}

/* F_1B213: the car showroom sprite sequence (strip sprites slide across the screen in both directions) */



void F_1B213(int *a, int *b)
{
    struct Blt blt;
    int left;
    struct Bitmap far *dst;
    int frame;
    int s72;
    struct Bitmap far *pic;
    int rev;
    int ybot;
    int right;
    int modelm;
    int attr3;
    int tinto;
    int n;
    struct Blt clip6;
    int sv401;
    int s74;

    sv401 = g_8240;
    s72 = g_8272.m8272;
    s74 = g_8272.m8274;
    F_193FE();
    F_120FE(-4);
    g_8240 = 0;
    g_6da0 = g_8b7c;
    modelm = g_7eaa.m7eb0->f2;
    attr3 = F_049A2(g_7eaa.m7eb0);
    tinto = (g_7eaa.m7eb0->f24 >> 8) & 0x1f;
    for (n = 0; n < 3; n++)
        g_6cda[n] = F_15BBE(g_5aaa[n], 1);
    F_1169A(0x88, 0xba - g_6cda[1]->height, g_6cda[1]->width, g_6cda[1]->height,
            (g_vid.mode != -2 && g_vid.mode != -6) ? g_02b4[1] : g_02c2[1]);
    g_5aa8 = (g_vid.mode == -6) ? 0x97 - g_6cda[0]->height : 0;
    g_6d28 = F_1934E(g_6cda[1]->width, 0xba - g_5aa8, 0, 1);
    F_11A72(g_vid.mask & 0x88, g_5aa8, g_6cda[1]->width, 0xba - g_5aa8, g_6d28);
    F_17BE6(&g_6df6, modelm, attr3, tinto, 0, 0, 0, 1);
    g_6dd4 = g_7682[modelm].b0;
    g_6cd8 = g_7682[modelm].b1;
    blt.w = g_6df6->width;
    blt.h = 0x1c;
    blt.sx = 0;
    blt.sy = ybot = g_6df6->height - 0x1c;
    g_6dd6 = F_1A89E(g_6df6, &blt, 1);
    F_118D4(0x408, -1);
    g_6dde = F_15BBE(0x49d, 2);
    F_19F42(g_6df6, &left, &right);
    g_6d34 = F_1934E(g_6df6->width, g_6dde->height, 0, 2);
    F_11A72(g_6da0, 0x99 - g_6dde->height, g_6df6->width, g_6dde->height, g_6d34);
    g_6dda = F_1934E(g_6dde->width + 8, g_6dde->height, 1, 2);
    F_1A03F(g_6dde, left & ~g_vid.mask, g_6dda);
    blt.w = g_6dda->width;
    blt.h = g_6dda->height;
    blt.sx = blt.sy = 0;
    blt.dx = left & g_vid.mask;
    blt.dy = 0;
    (*g_78aa)(g_6dda, g_6d34, &blt, 1);
    F_19438(1);
    g_6dda = F_1934E(g_6dde->width + 8, g_6dde->height, 1, 2);
    F_1A03F(g_6dde, (right + 1) & ~g_vid.mask, g_6dda);
    blt.w = g_6dda->width;
    blt.dx = right - blt.w + ~g_vid.mask + 1;
    (*g_78aa)(g_6dda, g_6d34, &blt, 1);
    F_19438(1);
    blt.w = g_6d34->width;
    blt.dx = g_6da0;
    blt.dy = 0x99 - blt.h;
    (*g_78a2)(g_6d34, g_vid.back, &blt, 1);
    F_19438(2);
    g_5aa8 = (g_vid.mode == -6) ? 0x92 - g_6df6->height : 0;
    g_6dfa = F_1934E(g_6df6->width, 0x99 - g_5aa8, 1, 1);
    F_11A72(g_6da0, g_5aa8, g_6df6->width, 0x99 - g_5aa8, g_6dfa);
    F_109C0(g_6dfa->alt.p, 0xff, g_6dfa->planesize / g_vid.planes);
    clip6.w = g_6cda[1]->width;
    clip6.h = g_6cda[1]->height;
    clip6.sx = clip6.sy = 0;
    clip6.dx = 0x88 - g_6da0;
    clip6.dy = -(g_6cda[1]->height + g_5aa8 - 0x9a);
    (*g_78aa)(g_6cda[0], g_6dfa, &clip6, 1);
    (*g_78a2)(g_6cda[0], g_6dfa, &clip6, 2);
    g_6d30 = F_15BBE(0x4c7, 2);
    g_5aa8 = (g_vid.mode == -6) ? 0x76 : 0;
    pic = F_1A9B2(g_6cd8);
    dst = F_1A9B2(g_6dd4);
    F_19438(1);
    g_6d24 = F_1934E(g_6df6->width + g_6da0, 0x1c, 0, 1);
    F_11A72(0, 0x80, g_6df6->width + g_6da0, 0x1c, g_6d24);
    blt.w = g_6df6->width;
    blt.h = g_6d24->height - 8;
    blt.sx = 0;
    blt.sy = g_6df6->height - 0x15;
    blt.dx = g_6da0;
    blt.dy = 0;
    (*g_78aa)(g_6df6, g_6d24, &blt, 1);
    blt.w = g_6cda[1]->width;
    blt.h = g_6d24->height;
    blt.sx = 0;
    blt.sy = g_6cda[1]->height - blt.h + 2;
    blt.dx = 0x88;
    blt.dy = 0;
    (*g_78aa)(g_6cda[2], g_6d24, &blt, 1);
    if (g_vid.mode == -2) {
        g_6cd4 = &g_6cf4;
        F_03904(g_6cd4, g_731e->bits.p, 0, g_6df6->width, 200, -2);
        F_03904(g_5ab0, g_731e->bits.p + g_6df6->pitch, 0, pic->width, 200, -2);
        F_03904(g_5ab4, g_7322->bits.p, 0, 0x140, g_6d24->height, -2);
        F_03904(g_5ab8, g_7322->bits.p, 0, g_6d28->width, 200, -2);
        blt.w = g_6dfa->width;
        blt.h = g_6df6->height + 4;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x95 - g_6df6->height;
        (*g_78a2)(g_6dfa, g_6cd4, &blt, 1);
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sy = blt.dy = 0x79;
        (*g_78a2)(dst, g_5ab0, &blt, 1);
        blt.w = g_6d24->width;
        blt.h = g_6d24->height;
        blt.sy = blt.dy = 0;
        (*g_78a2)(g_6d24, g_5ab4, &blt, 1);
        blt.w = g_6d28->width;
        blt.h = g_6cda[0]->height + 0x21;
        blt.sy = blt.dy = 0x99 - g_6cda[0]->height;
        (*g_78a2)(g_6d28, g_5ab8, &blt, 1);
    } else if (g_vid.mode != -6) {
        g_6cd4 = F_1934E(g_6dfa->width, g_6dfa->height, 0, 1);
        *g_5ab0 = *pic;
        g_5ab0->bits.p = g_6cd4->bits.p;
        g_5ab0->alt.p = 0;
        *g_5ab4 = *g_6d24;
        g_5ab4->bits.p = g_6cd4->bits.p;
        g_5ab4->alt.p = 0;
        *g_5ab8 = *g_6d28;
        g_5ab8->bits.p = g_6cd4->bits.p;
        g_5ab8->alt.p = 0;
    }
    g_5aa8 = (g_vid.mode == -6) ? 0x97 - g_6cda[0]->height : 0;
    if (g_vid.mode == -2)
        (*g_78b6)(g_7316[0]->bits.p, g_731a->bits.p);
    g_6d3c = F_195F3(0x88, 0xb9, &g_6cda[1], 0, 1, g_5ab8, g_6d28, 0, 0);
    F_199DD(g_6d3c, 0, 0x21, 0, -1);
    while ((*g_78c6)(g_6d3c))
        F_10AFD(3);
    F_16381(1);
    g_5aa8 = (g_vid.mode == -6) ? 0x92 - g_6df6->height : 0;
    g_6cf0 = F_15BBE(*a, 2);
    F_1A472(g_6cf0, g_6df6, g_6dd4, ybot);
    F_1A472(g_6cf0, g_6df6, g_6cd8, ybot);
    F_19438(1);
    if (g_vid.mode == -2)
        (*g_78b6)(g_7316[0]->bits.p, g_731a->bits.p);
    g_6ce8.x1 = 0;
    g_6ce8.y1 = 0;
    g_6ce8.x2 = g_6dfa->width - 1;
    g_6ce8.y2 = g_6dfa->height + g_5aa8 - 1;
    g_6d3c = F_195F3(g_6da0, 0x98, &g_6df6, 0, 1, g_6cd4, g_6dfa, &g_6ce8, 0);
    F_199DD(g_6d3c, 0, 7, 0, -1);
    frame = 0;
    rev = 0;
    F_169E1(1);
    while ((*g_78c6)(g_6d3c)) {
        F_10AFD(3);
        if (rev) {
            if (frame == 2) {
                frame = 1;
                rev = 0;
            } else
                frame++;
        } else {
            if (frame == 0) {
                frame = 1;
                rev = frame;
            } else
                frame--;
        }
        if (rev == 0)
            g_6d3c->y++;
        (*g_78aa)(g_6cda[frame], g_6dfa, &clip6, 1);
        (*g_78a2)(g_6cda[frame], g_6dfa, &clip6, 2);
    }
    F_169E1(0);
    g_5aa8 = (g_vid.mode == -6) ? 0x76 : 0;
    g_6d2c = dst;
    F_1ABAC(g_6da0 + g_6dd4, a, b);
    g_6d2c = pic;
    if (g_vid.mode == -2) {
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x79;
        (*g_78a2)(pic, g_5ab0, &blt, 1);
    }
    blt.w = 0x28;
    blt.h = g_6d24->height;
    blt.sx = blt.dx = g_6da0 + g_6dd4;
    blt.sy = 0x9c - blt.h;
    blt.dy = 0;
    (*g_78a2)(g_vid.front, g_6d24, &blt, 1);
    if (g_vid.mode == -2)
        (*g_78a2)(g_vid.front, g_5ab4, &blt, 1);
    F_1ABAC(g_6da0 + g_6cd8, a, b);
    g_5aa8 = (g_vid.mode == -6) ? 0x92 - g_6df6->height : 0;
    g_6de2[0] = F_15BBE(*b, 1);
    blt.w = g_6df6->width;
    blt.h = g_6de2[0]->height;
    blt.dy = g_6df6->height - g_6de2[0]->height;
    blt.sx = blt.sy = blt.dx = 0;
    (*g_78a2)(g_6dd6, g_6df6, &blt, 3);
    F_1A472(g_6de2[0], g_6df6, g_6dd4, ybot);
    F_1A472(g_6de2[0], g_6df6, g_6cd8, ybot);
    if (g_vid.mode == -2)
        (*g_78b6)(g_7316[0]->bits.p, g_731a->bits.p);
    frame = 2;
    rev = 1;
    (*g_78aa)(g_6cda[2], g_6dfa, &clip6, 1);
    (*g_78a2)(g_6cda[2], g_6dfa, &clip6, 2);
    g_6ce8.x1 = 0;
    g_6ce8.y1 = 0;
    g_6ce8.x2 = g_6dfa->width - 1;
    g_6ce8.y2 = g_6dfa->height + g_5aa8 - 1;
    g_6d3c = F_195F3(g_6da0, 0x94, &g_6df6, 0, 1, g_6cd4, g_6dfa, &g_6ce8, 0);
    F_199DD(g_6d3c, 0, 9, 0, 1);
    F_169E1(1);
    while ((*g_78c6)(g_6d3c)) {
        F_10AFD(3);
        if (rev) {
            if (frame == 2) {
                frame = 1;
                rev = 0;
            } else
                frame++;
        } else {
            if (frame == 0) {
                frame = 1;
                rev = frame;
            } else
                frame--;
        }
        if (rev == 0)
            g_6d3c->y--;
        (*g_78aa)(g_6cda[frame], g_6dfa, &clip6, 1);
        (*g_78a2)(g_6cda[frame], g_6dfa, &clip6, 2);
    }
    F_169E1(0);
    F_16381(0);
    g_5aa8 = (g_vid.mode == -6) ? 0x97 - g_6cda[0]->height : 0;
    if (g_vid.mode == -2)
        (*g_78b6)(g_7316[0]->bits.p, g_731a->bits.p);
    g_6d3c = F_195F3(0x88, 0x99, &g_6cda[1], 0, 1, g_5ab8, g_6d28, 0, 0);
    F_199DD(g_6d3c, 0, 0x21, 0, 1);
    while ((*g_78c6)(g_6d3c))
        F_10AFD(3);
    F_193FE();
    (*g_78b6)(g_vid.front->bits.p, g_vid.back->bits.p);
    g_8b7c = g_6da0;
    g_8240 = sv401;
    g_8272.m8272 = s72;
    g_8272.m8274 = s74;
    g_5aa8 = 0;
    F_120FE(-2);
}
