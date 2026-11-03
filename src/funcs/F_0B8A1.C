/* F_0B8A1 @ 0000:B8A1 (linear 0x0B8A1, 1587 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0B4DE(void);
static void near F_0B525(void);
static void near F_0B552(void);
static void near F_0B59B(void);
static void near F_0B617(void);
static void near F_0B61B(struct Dlg *, int);
static void near F_0B717(struct Dlg *);

void F_0B8A1(void)
{
    struct Bitmap far *label;
    struct Dlg dl;
    int pos;
    int wait1;
    int done;
    int key;

    F_11421(-1, "", 0);
    g_0284 = 3;
    F_118D4(0x442, -1);
    F_11593(g_49dc);
    g_8240 = 1;
    (*g_present)();
    dl.f0c = 1;
    F_120FE(1);
    dl.f0 = g_7eb0->f2;
    dl.f2 = F_049A2(g_7eb0);
    F_181C8(4, dl.f0, dl.f2, (g_7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[g_4808[g_7eb0->slot12->idx].kind]);
    F_16381(0);
    label = F_11A72(0x50, 0xaa, 0x28, 0xc, 0);
    dl.bm1 = F_11A72(g_826e - 0x10, g_8270 - 5, 0x18, 0xc, 0);
    g_8240 = 0;
    F_11BD3(g_0290, 0x40, 0x64);
    dl.bm2 = F_11A72(g_826e - 0x10, g_8270 - 5, 0x18, 0xc, 0);
    F_11BD3(g_028c, 0x40, 0x64);
    g_8240 = 1;
    pos = 1;
    dl.f1a = 0;
    wait1 = 0x28;
    done = 0;
    dl.f12 = 0;
    dl.f14 = 1;
    dl.f10 = 1;
    dl.f16 = 0;
    dl.f18 = 1;
    dl.f0e = (0xbe - g_7eb0->f22) / 0x1e;
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
                g_7eb0->f22 += 0x1e;
                done = 1;
                F_11540(-(pos <= g_7eaa ? pos : (int)g_7eaa));
                if (dl.f0e == 0 || pos > g_7eaa) {
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
                if (pos > g_7eaa || done) {
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
                if (dl.f0e > 0 && pos <= g_7eaa)
                    F_03AB8(0x18dd);
                goto both;
            }
            if (dl.f0e == 0 || pos > g_7eaa)
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
