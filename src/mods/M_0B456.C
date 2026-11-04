#include "STRUCTS.H"
#include "M_0B3CA.H"
#include "FONTDATA.H"
#include "GBSS.H"
#include "HOTSPOT.H"
#include "MESSAGES.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_02FC8.H"
#include "M_039C0.H"
#include "M_0542E.H"
#include "M_06476.H"
#include "M_08D26.H"
#include "M_08EA8.H"
#include "M_0914C.H"
#include "M_0E56A.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_11E9C.H"
#include "M_139DE.H"
#include "M_15236.H"
#include "M_15396.H"
#include "M_1628A.H"
#include "M_163BE.H"
#include "M_174A6.H"
#include "M_1923C.H"
#include "M_193D4.H"
#include "M_19ED0.H"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int g7644m;
char at787cu;
int h_8264;
int g826af;
int ae826cv;
int ar826e;
int q_8270;
int bp82c2;
int dz82c8x;
int s_8632;
float p_8bc4;

static int near F_0B3CA(int);
static void near F_0B456(void);
static void near F_0B49C(int, int, int, int, int);
static void near F_0B425(void);
static void near F_0B61B(struct Dlg *, int);
static void near F_0B4DE(void);
static void near F_0B525(void);
static void near F_0B552(void);
static void near F_0B59B(void);
static void near F_0B617(void);
static void near F_0B717(struct Dlg *);
static struct Node * near F_03FF0(int, int);
static int near F_04386(struct Node *, int);
static void near F_076A7(int, struct Node **, char *, int);
static void near F_07741(struct Node *, struct Node **);
static struct Node * near F_077B7(struct Node *);
static void near F_0C1D8(void);
static void near F_0C497(void);
static void near F_0C52C(int n);
static void near F_0C5AC(int n);
static int near F_0BED4(void);
static void near F_0BF94(void);
static struct Car * near F_04036(int idx, int weight, struct Car *old);
static int near F_043BF(struct Car *c, int keep);
static void near F_0DCBE(struct Car *c, struct Eng *e);

/* F_0B456 @ 0000:B456 (linear 0x0B456, 70 bytes) */



static void near F_0B456(void)
{
    int k;

    g_6ca6 = 0;
    g_6ca8 = 0;
    g_6ca4 = 0;
    for (k = 0; k < 6; k++)
        g_09f4[F_0B3CA(g_53b6[k])].f0 = 14;
}

/* F_0B49C @ 0000:B49C (linear 0x0B49C, 66 bytes) */



static void near F_0B49C(int id, int x, int y, int w, int h)
{
    struct Hot *p;
    p = &g_09f4[F_0B3CA(id)];
    p->x2 = (p->x1 = x) + w - 1;
    p->y2 = (p->y1 = y) + h - 1;
    p->f0 = 11;
}

/* F_0B4DE @ 0000:B4DE (linear 0x0B4DE, 71 bytes) */



static void near F_0B4DE(void)
{
    F_0B456();
    F_0B49C(2, 0x3f, 100, 12, 23);
    F_0B49C(1, 0x54, 0xa8, 0x1f, 15);
    F_0B425();
    g_6ca6 = 1;
    g_6ca8 = 1;
}

/* F_0B525 @ 0000:B525 (linear 0x0B525, 45 bytes) */



static void near F_0B525(void)
{
    F_0B456();
    F_0B49C(4, ar826e - 10, q_8270 - 5, 18, 12);
    F_0B425();
    g_6ca4 = 1;
}

/* F_0B552 @ 0000:B552 (linear 0x0B552, 73 bytes) */



static void near F_0B552(void)
{
    F_0B456();
    F_0B49C(2, 0x34, 100, 16, 23);
    F_0B49C(4, ar826e - 1, q_8270 - 1, 10, 9);
    F_0B425();
    g_6ca8 = 1;
    g_6ca4 = 1;
}

/* F_0B59B @ 0000:B59B (linear 0x0B59B, 124 bytes) */



static void near F_0B59B(void)
{
    F_0B456();
    if (h_8264)
        F_0B49C(6, g826af, ae826cv, 20, 19);
    F_0B49C(5, d_8266, ap8268t, 22, 19);
    F_0B49C(1, 0x3e, 0xa5, 0x35, 18);
    F_0B49C(3, 0xc4, 0x5a, 16, 14);
    F_0B425();
    g_6ca6 = 1;
}

/* F_0B617 @ 0000:B617 (linear 0x0B617, 4 bytes) */



static void near F_0B617(void)
{
    F_0B456();
}

/* F_0B61B @ 0000:B61B (linear 0x0B61B, 252 bytes) */


static void near F_0B61B(struct Dlg *d, int id)
{
    struct Bitmap far *back, far *label, far *work;
    struct Blt box;

    if (d->f14)
        back = d->bm1;
    else
        back = d->bm2;
    F_120FE(-3);
    label = F_15BBE(id, 2);
    F_120FE(-1);
    work = F_1934E(back->width, back->height, 0, 2);
    F_19ED0(back, work, 1);
    box.w = label->width;
    box.h = box.fc = label->height;
    box.sx = box.sy = 0;
    box.dx = (~r_vid.mask & ar826e) - box.w + 24;
    box.dy = 12 - label->height;
    F_1A2D0(label, work, &box, 0);
    F_11BD3(work, ar826e - 16, q_8270 - 5);
    F_19438(2);
}

/* F_0B717 @ 0000:B717 (linear 0x0B717, 394 bytes) */



static void near F_0B717(struct Dlg *d)
{
    struct Bitmap far *p;

    if (d->f0c != 1)
        F_120FE(1);
    if (d->f12) {
        F_0B61B(d, 0x472);
        if (d->f18) {
            F_11540(a_7eaap.m7eaa >= 10 ? -10 : -(int)a_7eaap.m7eaa);
            d->f18 = 0;
        }
    }
    if (d->f14 == 0) {
        F_11BD3(g_028c, 0x40, 0x64);
        if (d->f18)
            F_11540(a_7eaap.m7eaa >= 10 ? -10 : -(int)a_7eaap.m7eaa);
    }
    if (d->f10 == 0) {
        F_120FE(-3);
        p = F_15BBE(0x4d1, 2);
        F_120FE(-1);
        F_18ADB(p, 0x60, 0, 0x30, 0x40, 0xc8, 0x51, 0xa9, 2);
    }
    a_7eaap.m7eb0->f22 = a_7eaap.m7eb0->f22 > 0xaa ? 0xaa : a_7eaap.m7eb0->f22;
    F_181C8(2, d->f0, d->f2, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[g_4806[a_7eaap.m7eb0->slot12->idx].f2]);
    F_13F21(1);
    F_193D4();
    g_0284 = 1;
    F_08D26();
}

/* F_0B8A1 @ 0000:B8A1 (linear 0x0B8A1, 1587 bytes) */



void F_0B8A1(void)
{
    struct Bitmap far *label;
    struct Dlg dl;
    int pos;
    int wait1;
    int done;
    int key;

    F_11421(-1, "\0", 0);
    g_0284 = 3;
    F_118D4(0x442, -1);
    F_11593(g_49dc);
    r_vid.f16 = 1;
    (*m_present)();
    dl.f0c = 1;
    F_120FE(1);
    dl.f0 = a_7eaap.m7eb0->f2;
    dl.f2 = F_049A2(a_7eaap.m7eb0);
    F_181C8(4, dl.f0, dl.f2, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[((struct Row8 *)((char *)g_4806 + 2))[a_7eaap.m7eb0->slot12->idx].kind]);
    F_16381(0);
    label = F_11A72(0x50, 0xaa, 0x28, 0xc, 0);
    dl.bm1 = F_11A72(ar826e - 0x10, q_8270 - 5, 0x18, 0xc, 0);
    r_vid.f16 = 0;
    F_11BD3(g_0290, 0x40, 0x64);
    dl.bm2 = F_11A72(ar826e - 0x10, q_8270 - 5, 0x18, 0xc, 0);
    F_11BD3(g_028c, 0x40, 0x64);
    r_vid.f16 = 1;
    pos = 1;
    dl.f1a = 0;
    wait1 = 0x28;
    done = 0;
    dl.f12 = 0;
    dl.f14 = 1;
    dl.f10 = 1;
    dl.f16 = 0;
    dl.f18 = 1;
    dl.f0e = (0xbe - a_7eaap.m7eb0->f22) / 0x1e;
    F_0B61B(&dl, 0x472);
    F_0B4DE();
    F_13DE6(0x17);
    F_120FE(-2);
top:
    g_05cc = g_tick + 20;
    for (;;) {
        while ((key = F_01417(3000)) == 0)
            ;
        switch (key) {
        case 1000:
            if (dl.f0c == 3)
                goto top;
            if (dl.f1a) {
                dl.f0e--;
                a_7eaap.m7eb0->f22 += 0x1e;
                done = 1;
                F_11540(-(pos <= a_7eaap.m7eaa ? pos : (int)a_7eaap.m7eaa));
                if (dl.f0e == 0 || pos > a_7eaap.m7eaa) {
                    if (F_111F2(2)) {
                        F_16993(0);
                        F_10D6C(2);
                    }
                    if (dl.f10)
                        goto tail;
                fin:
                    F_0B717(&dl);
                    return;
                }
            } else {
                if (--wait1 > 0)
                    goto top;
                F_13F21(1);
                F_0B617();
                F_13DE6(0x17);
                pos <<= 1;
                dl.f16 = 1;
                F_04FCA(3);
                F_03AB8(0x19e1);
                dl.f10 = 0;
                if (done)
                    goto fin;
                dl.f18 = 0;
                if (dl.f0c == 1) {
                    F_11BD3(g_0290, 0x40, 0x64);
                    F_120FE(dl.f0c = 2);
                    dl.f14 = 0;
                }
                F_0B61B(&dl, 0x471);
                dl.f12 = 1;
                F_120FE(dl.f0c = 1);
                F_11149(2);
                F_16993(1);
                dl.f1a = 1;
            }
            g_05cc = g_tick + 0x4c;
            break;
        case 1:
            if (g_6ca6 == 0)
                break;
            if (dl.f0c == 1) {
                F_120FE(dl.f0c = 3);
                F_119B8(0x473, 0x50, 0xaa, -1);
                F_13F21(1);
                F_0B59B();
        redraw:
                F_13DE6(0x17);
                goto top;
            } else {
                F_120FE(dl.f0c = 1);
                F_11BD3(label, 0x50, 0xaa);
                F_13F21(1);
                F_0B4DE();
                goto redraw;
            }
        case 2:
            if (g_6ca8 == 0)
                break;
            if (dl.f0c == 1) {
                F_120FE(dl.f0c = 2);
                F_11BD3(g_0290, 0x40, 0x64);
                dl.f14 = 0;
            both:
                F_13F21(1);
                F_0B552();
            } else {
                if (pos > a_7eaap.m7eaa || done) {
                    dl.f18 = 0;
                    goto fin;
                }
                F_120FE(dl.f0c = 1);
                F_11BD3(g_028c, 0x40, 0x64);
                dl.f14 = 1;
                F_13F21(1);
                F_0B4DE();
            }
            F_13DE6(0x17);
        tail:
            g_05cc = g_tick + 20;
            dl.f1a = 0;
            break;
        case 4:
            if (g_6ca4 == 0)
                break;
            if (dl.f0c == 1) {
                if (F_111F2(2)) {
                    F_16993(0);
                    F_10D6C(2);
                }
                F_0B61B(&dl, 0x472);
                dl.f12 = 0;
                F_120FE(dl.f0c = 2);
                if (dl.f0e > 0 && pos <= a_7eaap.m7eaa)
                    F_03AB8(0x18dd);
                goto both;
            }
            if (dl.f0e == 0 || pos > a_7eaap.m7eaa)
                F_03AB8(0x10b5);
            else {
                F_0B61B(&dl, 0x471);
                dl.f12 = 1;
                F_120FE(dl.f0c = 1);
                F_11149(2);
                F_16993(1);
                F_13F21(1);
                F_0B525();
                F_13DE6(0x17);
                g_05cc = g_tick + 0x4c;
                dl.f1a = 1;
            }
            break;
        case 3:
            F_03AB8(0x10f6);
            continue;
        case 5:
            break;
        case 6:
            break;
        }
    }
}

/* F_0BED4 @ 0000:BED4 (linear 0x0BED4, 192 bytes) */



static int near F_0BED4(void)
{
    struct Node *tbl[142];
    int ret;

    ret = 0;
    if (a_7eaap.m7eb0 == 0)
        return 0;
    a_7eaap.m7eb0->weight -= F_04386(a_7eaap.m7eb0->slot12, 0);
    a_7eaap.m7eb0->slot12 = 0;
    F_076A7(4, tbl, 0, 0);
    if (g_49e0[0]) {
        F_07741(F_077B7(tbl[1]), &a_7eaap.m7eb0->slot12);
    } else {
        a_7eaap.m7eaa -= 0x32;
        if (a_7eaap.m7eaa < 0) {
            ret = (int)-a_7eaap.m7eaa;
            a_7eaap.m7eaa = 0;
        }
        F_03FF0(0x28, g_4806[40].f0);
        tbl[0] = a_7eaap.m7eb8;
        a_7eaap.m7eb8 = tbl[0]->next;
        F_07741(tbl[0], &a_7eaap.m7eb0->slot12);
    }
    return ret;
}

int g_53c8[6] = {4973, 5015, 5073, 5043, 5099, 5130};   /* DS:53C8 */
struct Spot g_53d4[3] = {{1264, 64, 84}, {1266, 208, 82}, {1265, 272, 123}};   /* DS:53D4 */
int g_53e6 = 0;   /* DS:53E6 */

/* F_0BF94 @ 0000:BF94 (linear 0x0BF94, 580 bytes) */


static void near F_0BF94(void)
{
    struct Bitmap far *a, far *b;
    int endk, desc3;
    char *p2;
    int wk;
    char d, e, cl;
    int maxw2;
    struct Blt pkt;

    F_193D4();
    a = F_15BBE(0x456, 1);
    b = F_15BBE(0x4ef, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.dx = pkt.sx = pkt.sy = 0;
    pkt.dy = 0x45;
    (*ab78aat)(b, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = b->width;
    pkt.dy = 0x14;
    (*b_78a2)(a, r_vid.back, &pkt, 1);
    maxw2 = a->width - 8;
    r_vid.f16 = 0;
    x_colors = bustyle->hilite;
    for (p2 = atnewName, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    (*a_78aem)(r_vid.back, 0x5e, 0x48, atnewName);
    *p2 = d;
    desc3 = g7d86n[a_7eaap.m7eb0->f2].f6;
    endk = desc3 + strcspn(g_2c01 + desc3 + 8, " `.") + 7;
    for (p2 = g_2c01 + desc3 + 1, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    e = g_2c01[endk + 1];
    g_2c01[endk + 1] = 0;
    (*a_78aem)(r_vid.back, 0x5e, 0x63, g_2c01 + desc3 + 1);
    g_2c01[endk + 1] = e;
    *p2 = d;
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.f16 = 1;
    F_120FE(-2);
    F_00E24();
}

/* F_0C1D8 @ 0000:C1D8 (linear 0x0C1D8, 559 bytes) */


static void near F_0C1D8(void)
{
    struct Bitmap far *a, far *b;
    struct Blt pkt;
    int fast, i;

    fast = r_vid.mode == -2 && g_0254 == 0;
    F_16BE3();
    F_193D4();
    a = F_15BBE(0x4eb, 1);
    b = F_15BBE(0x4ec, 1);
    pkt.w = a->width;
    pkt.h = 0x5a;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = 0xb;
    (*ab78aat)(a, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height - 0x59;
    pkt.sy = 0x59;
    pkt.sx = pkt.dx = 0;
    pkt.dy = (fast ? 0x5a : 0x59) + 0xb;
    (*ab78aat)(a, r_vid.back, &pkt, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = fast ? 0x78 : 0x77;
    (*b_78a2)(b, r_vid.back, &pkt, 1);
    F_10414(bp82c2);
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    if (e_78e0.f0 > 0x5a) {
        for (i = 0; i < 3; i++) {
            a = F_15BBE(g_53d4[i].id, 1);
            pkt.w = a->width;
            pkt.h = a->height;
            pkt.sx = pkt.sy = 0;
            pkt.dx = g_53d4[i].x;
            pkt.dy = g_53d4[i].y;
            F_10AFD(0x14 - 8 * i);
            (*ab78aat)(a, r_vid.back, &pkt, 1);
            (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
        }
    }
    F_120FE(-2);
    (*m_present)();
    F_00E24();
    F_03A83(0x64);
    F_00E24();
    F_193D4();
}

/* F_0C407 @ 0000:C407 (linear 0x0C407, 144 bytes) */


void F_0C407(void)
{
    if (g_53e6 == 0) {
        if ((ar7e8av.m7e8a - ao7d2ev.m7d30) * 8 + ar7e8av.m7e8c <= ao7d2ev.m7d32)
            s_8632 = 1;
        else
            s_8632 = 0;
        g7644m = b_82bc / e_78e0.fc0;
        p_8bc4 = e_78e0.fc0 / 6.068827333333334;
        dz82c8x = (int)((ao7d2ev.m7d30 - 200) * 100.0 / (ar7e8av.m7e8a - 200)) > 99 ? 99 : (int)((ao7d2ev.m7d30 - 200) * 100.0 / (ar7e8av.m7e8a - 200));
        g_53e6 = 1;
    }
}

/* F_0C497 @ 0000:C497 (linear 0x0C497, 149 bytes) */


static void near F_0C497(void)
{
    if (bo7676) {
        sprintf(g_2b3f, "Your``winning``time %2.1f sec.,`avg. speed %2d mph", p_8bc4, g7644m);
        strcpy(g_2b72, "Congratulations !");
    } else {
        sprintf(g_2b3f, "In %2.1f sec you covered only %2d%%", p_8bc4, dz82c8x);
        sprintf(g_2b72, "of the distance. Your avg. speed %2d mph.", g7644m);
    }
    F_13DE6(0x21);
    F_13F21(1);
}

/* F_0C52C @ 0000:C52C (linear 0x0C52C, 128 bytes) */



static void near F_0C52C(int n)
{
    struct Node **p;
    char *q;
    int i;

    if (a_7eaap.m7eb0) {
        for (i = 0, p = a_7eaap.m7eb0->rack.s; i < 5; i++, p++) {
            a_7eaap.m7eb0->weight -= F_04386(*p, 0);
            *p = 0;
        }
        for (i = 0, q = a_7eaap.m7eb0->b16; i < 9; i++)
            *q++ = 0;
        a_7eaap.m7eb0->f1f = 0;
    }
    F_00E24();
    F_16BC1();
    F_04FFA(-n, -1, 0x49);
}

/* F_0C5AC @ 0000:C5AC (linear 0x0C5AC, 103 bytes) */



static void near F_0C5AC(int n)
{
    char *q;
    int i;

    if (a_7eaap.m7eb0) {
        a_7eaap.m7eb0->weight -= F_04386(a_7eaap.m7eb0->slot6, 0);
        a_7eaap.m7eb0->slot6 = 0;
        for (i = 0, q = a_7eaap.m7eb0->b14; i < 2; i++)
            *q++ = 0;
    }
    F_00E24();
    F_16BC1();
    F_04FFA(-n, -1, 0x49);
}

int g_54b2[3][5] = {{2, 5, 8, 11, 14}, {0, 2, 4, 6, 8}, {0, 0, 1, 2, 4}};   /* DS:54B2 */
int g_54d0 = 0;   /* DS:54D0 */
int g_54d2 = 0;   /* DS:54D2 */
int g_54d4 = 1;   /* DS:54D4 */
int g_54d6 = 0;   /* DS:54D6 */
int g_54d8 = 0;   /* DS:54D8 */
int g_54da = 0;   /* DS:54DA */

/* F_0C613 @ 0000:C613 (linear 0x0C613, 1948 bytes) */



static int near F_0C613(void)
{
    int units;
    char opt;
    int sel;
    int kind;
    unsigned v;
    char stop;
    char *ptr;
    char temp[40];
    char engine;
    int all;
    int i;
    long cost;
    struct Car *found;
    int list[25];

    kind = as79a2z;
    v = d_79a4;
    engine = e_78e0.fbc & 4;
    opt = e_78e0.fbc & 0x10;
    stop = e_78e0.fbc == 5 || e_78e0.fbc == 0x11 || e_78e0.fbc == 0x15;
    if (a_7eaap.m7eb0->slot6->b > 9900)
        a_7eaap.m7eb0->slot6->b = 9900;
    if (a_7eaap.m7eb0->rack.s[0]->b > 9900)
        a_7eaap.m7eb0->rack.s[0]->b = 9900;
    if (a_7eaap.m7eb0->slot12->b > 9900)
        a_7eaap.m7eb0->slot12->b = 9900;
    bo7676 = 0;
    g_53e6 = 0;
    b_82bc = 0;
    g_47cd = 0;
    g_4732 = 0;
    if (e_78e0.f20)
        e_78e0.f24 = 1;
    if (at787cu)
        e_78e0.fbc |= 8;
    F_13DE6(1);
    i = a_7eaap.m7ec0 == 0 ? 5 : 15;
    if (a_7eaap.m7eb0->f22 > i)
        a_7eaap.m7eb0->f22 -= i;
    else
        a_7eaap.m7eb0->f22 = 0;
    (*m_present)();
    if (g79e2g.fbc & 1) {
        bo7676 = 1;
        F_0C497();
        F_120FE(-2);
        F_03AB8(g7d40q == 0 ? (i = g_53c8[kind]) : a_7eaap.m7ec0 == 0 ? 0x14bd : 0x14d6);
        F_11540(v);
        if (kind == 5) {
            found = F_04036(bn7648, ar7d7au->f0, 0);
            found->slot12->b = random_int(500);
            found->f24 = ((g7d84o->ff & 0x1f) << 8) | (found->f24 & 0xe0ff);
            found->f24 = (((g7d84o->fe & 4) != 0 & 1) << 13) | (found->f24 & 0xdfff);
            found->f24 = (((g7d84o->fe & 2) != 0 & 1) << 14) | (found->f24 & 0xbfff);
            found->f24 = (((g7d84o->fe & 1) != 0 & 1) << 15) | (found->f24 & 0x7fff);
            found->f4 = g7d84o->fa;
            if (found->f5 > a_7eaap.m7eb0->f5) {
                a_7eaap.m7eb0->f26 = a_7eaap.m7eb2->f26;
                a_7eaap.m7eb2 = a_7eaap.m7eb0;
                a_7eaap.m7eb0 = found;
                F_0DCBE(a_7eaap.m7eb0, &e_78e0);
            }
            if (g_0610 == 0 || g7f82r != g_4ff4) {
                if (g7d84o->f10 != 0 || g7d40q != 0)
                    g7d84o->fb = 0xff;
                else {
                    for (i = 0, units = 0; i < 25; i++)
                        if (g7d86n[i].f2 < ar7d7au->f2)
                            list[++units] = i;
                    if (units > 0) {
                        list[0] = units;
                        F_0542E(g7f82r, list);
                        g7d84o->f10 = 1;
                    } else
                        g7d84o->fb = 0xff;
                    goto done;
                }
            }
        }
done:
        F_13F21(1);
        if (e_78e0.fbc & 8)
            if ((i = F_0BED4()) != 0)
                goto ret2;
        if (kind == 5)
            return 1;
        return 3;
    } else if (e_78e0.fbc & 0x40) {
        F_16AA8(0);
        F_0BF94();
        i = (e_78e0.fbc & 8) ? F_0BED4() : 0;
        if (e_78e0.f0 > 0x50)
            i += 0x4b;
        else if (e_78e0.f0 > 0x32)
            i += 0x32;
        else
            i += 0x14;
        F_13DE6(0x18);
        F_01417(0);
        sprintf(temp, "You have been fined $%2d for speeding", i);
        F_11421(-1, temp, 0x140 - o_8250);
        F_03A83(0xfa);
        if (i <= a_7eaap.m7eaa)
            goto pay;
        F_03AB8(0x1793);
        F_13F21(1);
        F_13F21(1);
        F_11540(-a_7eaap.m7eaa);
        F_11421(-1, " ", 0x140 - o_8250);
        return 4;
pay:
        F_11540(-i);
        cost = g_05cc;
        F_03A83(g_05dc);
        g_05cc = cost;
        F_13F21(1);
        F_11421(-1, "", 0x140 - o_8250);
    } else if (e_78e0.fbc == 0x81) {
        F_120FE(-2);
        F_03AB8(0xecb2);
    } else {
        if (kind == 5) {
            F_043BF(a_7eaap.m7eb0, 0);
            a_7eaap.m7eb0 = 0;
        }
        F_11540(-v);
        if (e_78e0.fbc == 1) {
            F_0C497();
            F_120FE(-2);
            i = kind == 5 ? 0x143a : 0x1427;
            F_03AB8(g7d40q == 0 ? i : a_7eaap.m7ec0 == 0 ? 0x1450 : 0x146c);
            F_13F21(1);
            if (kind != 5)
                return 3;
ret2:
            return 2;
        } else if (stop) {
            F_120FE(-2);
            if (engine) {
                F_0C52C(kind == 5 ? 0x16bd : 0x1634);
                goto finishstop;
            } else if (opt) {
                F_0C5AC(kind == 5 ? 0x1678 : 0x1608);
                goto finishstop;
            }
finishstop:
            goto fin;
        } else {
            F_0C1D8();
            if (kind == 5) {
                if (e_78e0.fbc & 8) {
                    F_03AB8(0x16f6);
                    F_0BED4();
                } else
                    F_03AB8(0x1658);
            } else if (e_78e0.f0 > 0x5a) {
                v = F_043BF(a_7eaap.m7eb0, 0) / 10 + 0x13;
                a_7eaap.m7eb0 = 0;
                F_11540(v);
                ptr = g_4115;
                while (*ptr++ != '$')
                    ;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                F_00E24();
                F_03AB8(0x1513);
            } else {
                if (e_78e0.f0 > 0x3c) {
                    i = 0;
                    v = random_int(0x28) + 100;
                } else if (e_78e0.f0 > 0x1e) {
                    i = 1;
                    v = random_int(0x14) + 100;
                } else {
                    if (e_78e0.fbc & 8) {
                        F_03AB8(0x1590);
                        all = F_0BED4();
                        goto fin;
                    }
                    F_03AB8(0x1561);
                    F_13F21(1);
                    return 1;
                }
                ptr = strchr(i ? g_2b09 : g_2ab2, '$') + 1;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                all = e_78e0.fbc & 8;
                if (all)
                    strcpy(g_2aea, "Plus tires !");
                else
                    g_2aea[0] = 0;
                sel = i ? 0x1a : 0x19;
                if ((long)v > a_7eaap.m7eaa) {
                    g_1ac2 = (*(int *)&g_1ac4[62]) = 0xe;
                    g_579f[sel][0].val = 0xd0;
                    g_579f[sel][1].key = 1;
                    g_579f[sel][1].val = 0xd0;
                } else {
                    g_1ac2 = (*(int *)&g_1ac4[62]) = 0xa;
                    g_579f[sel][0].val = 0xcf;
                    g_579f[sel][1].key = 0x21;
                    g_579f[sel][1].val = 0xcf;
                }
                F_13DE6(sel);
                F_00E24();
                while ((i = F_01417(3000)) == 0)
                    ;
                F_13F21(1);
                switch (i) {
                case 1:
                    F_11540(-v);
                    if (all)
                        F_0BED4();
                    F_13F21(1);
                    return 1;
                case 2:
                    F_11540((int)(a_7eaap.m7eb0->weight * 0.4));
                    a_7eaap.m7eb0 = 0;
                    break;
                default:
                    return;
                }
            }
        }
fin:
        F_13F21(1);
        goto ret2;
    }
    F_13F21(1);
    return 3;
}
