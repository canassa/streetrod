/* F_1ABAC: build the four shifted copies of a background strip and animate the sprite over them */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

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
