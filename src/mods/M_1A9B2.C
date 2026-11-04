#include "STRUCTS.H"
#include "M_1A9B2.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_036A8.H"
#include "M_039C0.H"
#include "M_06476.H"
#include "M_0914C.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_15396.H"
#include "M_1628A.H"
#include "M_163BE.H"
#include "M_174A6.H"
#include "M_1923C.H"
#include "M_193D4.H"
#include "M_1947A.H"
#include "M_19ED0.H"

static struct Bitmap far *g_6cd4oz;        /* DS:6CD4 */
static int g_6cd8lz;                       /* DS:6CD8 */
static struct Bitmap far *g_6cdaaa[3];     /* DS:6CDA */
static int g_6ce6sz;                       /* DS:6CE6 */
static struct Rect g_6ce8rz;               /* DS:6CE8 */
static struct Bitmap far *g_6cf0zz;        /* DS:6CF0 */
static struct Bitmap g_6cf4wz;             /* DS:6CF4 */
static struct Bitmap far *g_6d24ajz;       /* DS:6D24 */
static struct Bitmap far *g_6d28agz;       /* DS:6D28 */
static struct Bitmap far *g_6d2c0nz;       /* DS:6D2C */
static struct Bitmap far *g_6d30;          /* DS:6D30 */
static struct Bitmap far *g_6d34;          /* DS:6D34 */
static struct Bitmap far *g_6d38;          /* DS:6D38 */
static struct Sprite *g_6d3caat;           /* DS:6D3C */
static int g_6d3eaas;                      /* DS:6D3E */
static struct Bitmap g_6d400;              /* DS:6D40 */
static struct Bitmap g_6d700;              /* DS:6D70 */
static int g_6da0;                         /* DS:6DA0 */
static int g_6da2;                         /* DS:6DA2 */
static struct Bitmap g_6da4;               /* DS:6DA4 */
static int g_6dd4;                         /* DS:6DD4 */
static struct Bitmap far *g_6dd6;          /* DS:6DD6 */
static struct Bitmap far *g_6dda;          /* DS:6DDA */
static struct Bitmap far *g_6dde;          /* DS:6DDE */
static struct Bitmap far *g_6de2a[4];      /* DS:6DE2 */
static struct Bitmap far *g_6df2a;         /* DS:6DF2 */
static struct Bitmap far *g_6df6a;         /* DS:6DF6 */
static struct Bitmap far *g_6dfa7;         /* DS:6DFA */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */

static struct Bitmap far * near F_1A9B2(int n);
static void near F_1ABAC(int a, int *p, int *q);

int g_5aa8 = 0;   /* DS:5AA8 */
int g_5aaa[3] = {1034, 1035, 1036};   /* DS:5AAA */
struct Bitmap far *g_5ab0 = (struct Bitmap far *)&g_6da4;   /* DS:5AB0 */
struct Bitmap far *g_5ab4 = (struct Bitmap far *)&g_6d400;   /* DS:5AB4 */
struct Bitmap far *g_5ab8 = (struct Bitmap far *)&g_6d700;   /* DS:5AB8 */

/* F_1A9B2: build the two scratch bitmaps for a shifted background strip and its 0x28-wide column */


static struct Bitmap far *near F_1A9B2(int n)
{
    struct Bitmap far *b;
    struct Blt blt;
    struct Bitmap far *a2;
    struct Bitmap far *c;

    a2 = F_1934E(g_6d30->width + 8, g_6d30->height, g_6d30->alt.p != 0L, 1);
    F_1A03F(g_6d30, n & ~r_vid.mask, a2);
    b = F_1934E(a2->width, a2->height, 0, 1);
    F_11A72((n + g_6da0) & r_vid.mask, 0x78, a2->width, a2->height, b);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*ab78aat)(a2, b, &blt, 1);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = 0;
    blt.dx = (n + g_6da0) & r_vid.mask;
    blt.dy = 0x78;
    (*b_78a2)(b, r_vid.back, &blt, 1);
    F_1941E(2);
    blt.w = 0x28;
    blt.h = g_6df6a->height;
    blt.sx = n & r_vid.mask;
    blt.sy = blt.dx = 0;
    blt.dy = -(g_6df6a->height + g_5aa8 - 0x95);
    c = F_1934E(0x28, 0x9c - g_5aa8, 1, 1);
    F_11A72(blt.sx + g_6da0, g_5aa8, 0x28, 0x9c - g_5aa8, c);
    F_109C0(c->alt.p, 0xff, c->planesize / r_vid.planes);
    (*ab78aat)(g_6df6a, c, &blt, 1);
    (*b_78a2)(g_6df6a, c, &blt, 2);
    return c;
}

/* F_1ABAC: build the four shifted copies of a background strip and animate the sprite over them */


static void near F_1ABAC(int a, int *p, int *q)
{
    int i;

    g_6d38 = F_15BBE(0x4d2, 1);
    g_6cf0zz = F_15BBE(*p, 1);
    g_6d3eaas = a & ~r_vid.mask;
    g_6de2a[0] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
    F_1A03F(g_6cf0zz, g_6d3eaas, g_6de2a[0]);
    g_6df2a = F_1934E(g_6de2a[0]->width, g_6de2a[0]->height, 1, 1);
    F_19ED0(g_6de2a[0], g_6df2a, 3);
    g_6ce6sz = g_6de2a[0]->height - g_6d38->height;
    F_1A472(g_6d38, g_6de2a[0], g_6d3eaas, g_6ce6sz);
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_6d2c0nz->width - 1;
    g_6ce8rz.y2 = g_6d2c0nz->height + g_5aa8 - 1;
    g_6d3caat = F_195F3(a & r_vid.mask, 0x94, &g_6df2a, 0, 1, g_5ab0, g_6d2c0nz, &g_6ce8rz, 0);
    F_199DD(g_6d3caat, 0, 8, 0, 1);
    while ((*g78c6c)(g_6d3caat)) {
        if (g_6d3caat->y == 0x7d)
            *g_6df2a = *g_6de2a[0];
        F_10AFD(3);
    }
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2 + g_6d3eaas) & ~r_vid.mask;
        if (g_6da2) {
            g_6cf0zz = F_15BBE(p[i], 2);
            g_6de2a[4 - i] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
            F_1A03F(g_6cf0zz, g_6da2, g_6de2a[4 - i]);
            F_19438(1);
        } else
            g_6de2a[4 - i] = F_15BBE(p[i], 1);
        F_1A472(g_6d38, g_6de2a[4 - i], g_6da2, g_6ce6sz);
    }
    i = 4;
    g_6d3caat = F_195F3(a, 0x9b, g_6de2a, 0, 4, g_5ab4, g_6d24ajz, 0, 0);
    F_199DD(g_6d3caat, 0, (a + g_6de2a[0]->width) >> 1, -2, 0);
    F_168C8(1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(2);
    F_168C8(0);
    F_1941E(6);
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6de2a[0] = F_15BBE(*q, 1);
    g_6d3eaas = ((unsigned char)a & 7) >> 1;
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2) & ~r_vid.mask;
        if (g_6da2) {
            g_6cf0zz = F_15BBE(q[i], 2);
            g_6de2a[i] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
            F_1A03F(g_6cf0zz, g_6da2, g_6de2a[i]);
            F_19438(1);
        } else
            g_6de2a[i] = F_15BBE(q[i], 1);
    }
    for (i = 0; i < 4; i++) {
        if (i == g_6d3eaas) {
            g_6df2a = F_1934E(g_6de2a[i]->width, g_6de2a[i]->height, 1, 1);
            F_19ED0(g_6de2a[i], g_6df2a, 3);
        }
        F_1A472(g_6d38, g_6de2a[i], (i * 2) & ~r_vid.mask, g_6ce6sz);
    }
    i = 4;
    g_6d3caat = F_195F3(-g_6de2a[0]->width, 0x9b, g_6de2a, 0, 4, g_5ab4, g_6d24ajz, 0, 0);
    F_199DD(g_6d3caat, 0, (a + g_6de2a[0]->width >> 1) + 1, 2, 0);
    F_168C8(1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(2);
    F_168C8(0);
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_6d2c0nz->width - 1;
    g_6ce8rz.y2 = g_6d2c0nz->height + g_5aa8 - 1;
    g_6d3caat = F_195F3(a, 0x9b, &g_6de2a[g_6d3eaas], 0, 1, g_5ab0, g_6d2c0nz, &g_6ce8rz, 0);
    F_199DD(g_6d3caat, 0, 8, 0, -1);
    while ((*g78c6c)(g_6d3caat)) {
        if (g_6d3caat->y == 0x7d)
            *g_6de2a[g_6d3eaas] = *g_6df2a;
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

    sv401 = r_vid.f16;
    s72 = m_8272.m8272;
    s74 = m_8272.m8274;
    F_193FE();
    F_120FE(-4);
    r_vid.f16 = 0;
    g_6da0 = z_8b7c;
    modelm = a_7eaap.m7eb0->f2;
    attr3 = F_049A2(a_7eaap.m7eb0);
    tinto = (a_7eaap.m7eb0->f24 >> 8) & 0x1f;
    for (n = 0; n < 3; n++)
        g_6cdaaa[n] = F_15BBE(g_5aaa[n], 1);
    F_1169A(0x88, 0xba - g_6cdaaa[1]->height, g_6cdaaa[1]->width, g_6cdaaa[1]->height,
            (r_vid.mode != -2 && r_vid.mode != -6) ? g_02b4[1] : g_02c2[1]);
    g_5aa8 = (r_vid.mode == -6) ? 0x97 - g_6cdaaa[0]->height : 0;
    g_6d28agz = F_1934E(g_6cdaaa[1]->width, 0xba - g_5aa8, 0, 1);
    F_11A72(r_vid.mask & 0x88, g_5aa8, g_6cdaaa[1]->width, 0xba - g_5aa8, g_6d28agz);
    F_17BE6(&g_6df6a, modelm, attr3, tinto, 0, 0, 0, 1);
    g_6dd4 = g7682n[modelm].b0;
    g_6cd8lz = g7682n[modelm].b1;
    blt.w = g_6df6a->width;
    blt.h = 0x1c;
    blt.sx = 0;
    blt.sy = ybot = g_6df6a->height - 0x1c;
    g_6dd6 = F_1A89E(g_6df6a, &blt, 1);
    F_118D4(0x408, -1);
    g_6dde = F_15BBE(0x49d, 2);
    F_19F42(g_6df6a, &left, &right);
    g_6d34 = F_1934E(g_6df6a->width, g_6dde->height, 0, 2);
    F_11A72(g_6da0, 0x99 - g_6dde->height, g_6df6a->width, g_6dde->height, g_6d34);
    g_6dda = F_1934E(g_6dde->width + 8, g_6dde->height, 1, 2);
    F_1A03F(g_6dde, left & ~r_vid.mask, g_6dda);
    blt.w = g_6dda->width;
    blt.h = g_6dda->height;
    blt.sx = blt.sy = 0;
    blt.dx = left & r_vid.mask;
    blt.dy = 0;
    (*ab78aat)(g_6dda, g_6d34, &blt, 1);
    F_19438(1);
    g_6dda = F_1934E(g_6dde->width + 8, g_6dde->height, 1, 2);
    F_1A03F(g_6dde, (right + 1) & ~r_vid.mask, g_6dda);
    blt.w = g_6dda->width;
    blt.dx = right - blt.w + ~r_vid.mask + 1;
    (*ab78aat)(g_6dda, g_6d34, &blt, 1);
    F_19438(1);
    blt.w = g_6d34->width;
    blt.dx = g_6da0;
    blt.dy = 0x99 - blt.h;
    (*b_78a2)(g_6d34, r_vid.back, &blt, 1);
    F_19438(2);
    g_5aa8 = (r_vid.mode == -6) ? 0x92 - g_6df6a->height : 0;
    g_6dfa7 = F_1934E(g_6df6a->width, 0x99 - g_5aa8, 1, 1);
    F_11A72(g_6da0, g_5aa8, g_6df6a->width, 0x99 - g_5aa8, g_6dfa7);
    F_109C0(g_6dfa7->alt.p, 0xff, g_6dfa7->planesize / r_vid.planes);
    clip6.w = g_6cdaaa[1]->width;
    clip6.h = g_6cdaaa[1]->height;
    clip6.sx = clip6.sy = 0;
    clip6.dx = 0x88 - g_6da0;
    clip6.dy = -(g_6cdaaa[1]->height + g_5aa8 - 0x9a);
    (*ab78aat)(g_6cdaaa[0], g_6dfa7, &clip6, 1);
    (*b_78a2)(g_6cdaaa[0], g_6dfa7, &clip6, 2);
    g_6d30 = F_15BBE(0x4c7, 2);
    g_5aa8 = (r_vid.mode == -6) ? 0x76 : 0;
    pic = F_1A9B2(g_6cd8lz);
    dst = F_1A9B2(g_6dd4);
    F_19438(1);
    g_6d24ajz = F_1934E(g_6df6a->width + g_6da0, 0x1c, 0, 1);
    F_11A72(0, 0x80, g_6df6a->width + g_6da0, 0x1c, g_6d24ajz);
    blt.w = g_6df6a->width;
    blt.h = g_6d24ajz->height - 8;
    blt.sx = 0;
    blt.sy = g_6df6a->height - 0x15;
    blt.dx = g_6da0;
    blt.dy = 0;
    (*ab78aat)(g_6df6a, g_6d24ajz, &blt, 1);
    blt.w = g_6cdaaa[1]->width;
    blt.h = g_6d24ajz->height;
    blt.sx = 0;
    blt.sy = g_6cdaaa[1]->height - blt.h + 2;
    blt.dx = 0x88;
    blt.dy = 0;
    (*ab78aat)(g_6cdaaa[2], g_6d24ajz, &blt, 1);
    if (r_vid.mode == -2) {
        g_6cd4oz = &g_6cf4wz;
        F_03904(g_6cd4oz, bp7316[2]->bits.p, 0, g_6df6a->width, 200, -2);
        F_03904(g_5ab0, bp7316[2]->bits.p + g_6df6a->pitch, 0, pic->width, 200, -2);
        F_03904(g_5ab4, bp7316[3]->bits.p, 0, 0x140, g_6d24ajz->height, -2);
        F_03904(g_5ab8, bp7316[3]->bits.p, 0, g_6d28agz->width, 200, -2);
        blt.w = g_6dfa7->width;
        blt.h = g_6df6a->height + 4;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x95 - g_6df6a->height;
        (*b_78a2)(g_6dfa7, g_6cd4oz, &blt, 1);
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sy = blt.dy = 0x79;
        (*b_78a2)(dst, g_5ab0, &blt, 1);
        blt.w = g_6d24ajz->width;
        blt.h = g_6d24ajz->height;
        blt.sy = blt.dy = 0;
        (*b_78a2)(g_6d24ajz, g_5ab4, &blt, 1);
        blt.w = g_6d28agz->width;
        blt.h = g_6cdaaa[0]->height + 0x21;
        blt.sy = blt.dy = 0x99 - g_6cdaaa[0]->height;
        (*b_78a2)(g_6d28agz, g_5ab8, &blt, 1);
    } else if (r_vid.mode != -6) {
        g_6cd4oz = F_1934E(g_6dfa7->width, g_6dfa7->height, 0, 1);
        *g_5ab0 = *pic;
        g_5ab0->bits.p = g_6cd4oz->bits.p;
        g_5ab0->alt.p = 0;
        *g_5ab4 = *g_6d24ajz;
        g_5ab4->bits.p = g_6cd4oz->bits.p;
        g_5ab4->alt.p = 0;
        *g_5ab8 = *g_6d28agz;
        g_5ab8->bits.p = g_6cd4oz->bits.p;
        g_5ab8->alt.p = 0;
    }
    g_5aa8 = (r_vid.mode == -6) ? 0x97 - g_6cdaaa[0]->height : 0;
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6d3caat = F_195F3(0x88, 0xb9, &g_6cdaaa[1], 0, 1, g_5ab8, g_6d28agz, 0, 0);
    F_199DD(g_6d3caat, 0, 0x21, 0, -1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(3);
    F_16381(1);
    g_5aa8 = (r_vid.mode == -6) ? 0x92 - g_6df6a->height : 0;
    g_6cf0zz = F_15BBE(*a, 2);
    F_1A472(g_6cf0zz, g_6df6a, g_6dd4, ybot);
    F_1A472(g_6cf0zz, g_6df6a, g_6cd8lz, ybot);
    F_19438(1);
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_6dfa7->width - 1;
    g_6ce8rz.y2 = g_6dfa7->height + g_5aa8 - 1;
    g_6d3caat = F_195F3(g_6da0, 0x98, &g_6df6a, 0, 1, g_6cd4oz, g_6dfa7, &g_6ce8rz, 0);
    F_199DD(g_6d3caat, 0, 7, 0, -1);
    frame = 0;
    rev = 0;
    F_169E1(1);
    while ((*g78c6c)(g_6d3caat)) {
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
            g_6d3caat->y++;
        (*ab78aat)(g_6cdaaa[frame], g_6dfa7, &clip6, 1);
        (*b_78a2)(g_6cdaaa[frame], g_6dfa7, &clip6, 2);
    }
    F_169E1(0);
    g_5aa8 = (r_vid.mode == -6) ? 0x76 : 0;
    g_6d2c0nz = dst;
    F_1ABAC(g_6da0 + g_6dd4, a, b);
    g_6d2c0nz = pic;
    if (r_vid.mode == -2) {
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x79;
        (*b_78a2)(pic, g_5ab0, &blt, 1);
    }
    blt.w = 0x28;
    blt.h = g_6d24ajz->height;
    blt.sx = blt.dx = g_6da0 + g_6dd4;
    blt.sy = 0x9c - blt.h;
    blt.dy = 0;
    (*b_78a2)(r_vid.front, g_6d24ajz, &blt, 1);
    if (r_vid.mode == -2)
        (*b_78a2)(r_vid.front, g_5ab4, &blt, 1);
    F_1ABAC(g_6da0 + g_6cd8lz, a, b);
    g_5aa8 = (r_vid.mode == -6) ? 0x92 - g_6df6a->height : 0;
    g_6de2a[0] = F_15BBE(*b, 1);
    blt.w = g_6df6a->width;
    blt.h = g_6de2a[0]->height;
    blt.dy = g_6df6a->height - g_6de2a[0]->height;
    blt.sx = blt.sy = blt.dx = 0;
    (*b_78a2)(g_6dd6, g_6df6a, &blt, 3);
    F_1A472(g_6de2a[0], g_6df6a, g_6dd4, ybot);
    F_1A472(g_6de2a[0], g_6df6a, g_6cd8lz, ybot);
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    frame = 2;
    rev = 1;
    (*ab78aat)(g_6cdaaa[2], g_6dfa7, &clip6, 1);
    (*b_78a2)(g_6cdaaa[2], g_6dfa7, &clip6, 2);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_6dfa7->width - 1;
    g_6ce8rz.y2 = g_6dfa7->height + g_5aa8 - 1;
    g_6d3caat = F_195F3(g_6da0, 0x94, &g_6df6a, 0, 1, g_6cd4oz, g_6dfa7, &g_6ce8rz, 0);
    F_199DD(g_6d3caat, 0, 9, 0, 1);
    F_169E1(1);
    while ((*g78c6c)(g_6d3caat)) {
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
            g_6d3caat->y--;
        (*ab78aat)(g_6cdaaa[frame], g_6dfa7, &clip6, 1);
        (*b_78a2)(g_6cdaaa[frame], g_6dfa7, &clip6, 2);
    }
    F_169E1(0);
    F_16381(0);
    g_5aa8 = (r_vid.mode == -6) ? 0x97 - g_6cdaaa[0]->height : 0;
    if (r_vid.mode == -2)
        (*dz78b6)(bp7316[0]->bits.p, bp7316[1]->bits.p);
    g_6d3caat = F_195F3(0x88, 0x99, &g_6cdaaa[1], 0, 1, g_5ab8, g_6d28agz, 0, 0);
    F_199DD(g_6d3caat, 0, 0x21, 0, 1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(3);
    F_193FE();
    (*dz78b6)(r_vid.front->bits.p, r_vid.back->bits.p);
    z_8b7c = g_6da0;
    r_vid.f16 = sv401;
    m_8272.m8272 = s72;
    m_8272.m8274 = s74;
    g_5aa8 = 0;
    F_120FE(-2);
}
