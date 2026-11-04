#include <string.h>
#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static struct Car * near F_04036(int idx, int weight, struct Car *old);
static struct Node * near F_03FF0(int idx, int a);

/* F_0442B @ 0000:442B (linear 0x0442B, 1301 bytes) */



static void near F_0442B(int tab)
{
    char list4[70];
    struct Bitmap far *bm1;
    char *p1;
    int off6;
    int used;
    struct Car *car;
    int sv;
    int x1;
    int y;
    int price6;
    struct Spec *specn;
    int n0;
    int ck;
    int i5;
    int k3;
    int n;
    char color;

    color = 0;
    g_0284 = 5;
    g_49e0[1] = 0xcd8;
    n = 2;
    i5 = price6 = 0;
    for (k3 = a_7d16q; k3 >= 0; k3 = g7d86n[k3].next) {
        g_49e0[n++] = g7d86n[k3].f6;
        list4[i5++] = k3;
    }
    g_49e0[n++] = 0xcfd;
    n0 = n;
    off6 = i5 - n0;
    for (k3 = 0; k3 < 0x2c; k3++) {
        if (g_4806[k3].f0 >= 0x14) {
            g_49e0[n++] = g_4806[k3].f6;
            list4[i5++] = k3;
        }
    }
    g_49e0[0] = n - 1;
    F_008B4(g_49e0, (tab == 1) ? 2 : 0x1c);
    goto test;
    do {
        used = n >= n0;
        if (used) {
            k3 = g_4806[i5 = list4[n + off6]].f6;
            price6 = g_4806[i5].f0;
            g_11b0 = 0xe;
        } else {
            k3 = (specn = &g7d86n[i5 = list4[n - 2]])->f6;
            price6 = specn->f0;
            g_11b0 = 10;
        }
        strcpy(g_2990, g_2c02 + k3);
        sprintf(g_29d6, "$%-3d ?", price6);
        color = -1;
        for (;;) {
            n = F_14294(9);
            if (n == -20) {
                if (g_0610 && (long)price6 > a_7eaap.m7eaa)
                    a_7eaap.m7eaa += 1999;
                if ((long)price6 > a_7eaap.m7eaa) {
                    p1 = strchr(g_3a5a, '$');
                    sprintf(++p1, "%d short.", price6 - (int)a_7eaap.m7eaa);
                    F_03AB8(0xf1a8);
                    break;
                }
                F_11540(-price6);
                if (used) {
                    F_03FF0(i5, price6);
                    g_05fc += 0x222;
                    if (a_7eaap.m7ebc > 0x82)
                        goto done;
                    break;
                }
                car = F_04036(i5, price6, 0);
                car->f4 = (color != -1) ? color : random_int(5);
                F_03AB8(0xf2de);
                g_05fc += 0x444;
                goto done;
            }
            if (n != -25)
                break;
            F_120FE(-4);
            F_193D4();
            bm1 = F_1934E(0x140, 0x52, 0, 0);
            F_11A72(0, 0x40, 0x140, 0x52, bm1);
            x_colors = bustyle->colors;
            ck = (r_vid.mode == -2 || r_vid.mode == -6) ? g_02c2[5] : g_02b4[5];
            F_1169A(0, 0x41, 0x140, 0x46, ck);
            F_1169A(0, 0x87, 0x140, 0xa, x_colors.c[1]);
            if (r_vid.mode != -2 && r_vid.mode != -6) {
                sv = x_colors.c[3];
                x_colors.c[3] = 3;
                F_1371C(r_vid.back, 0, 0x40, 0x52);
                F_1371C(r_vid.back, 0x13f, 0x40, 0x52);
                F_1371C(r_vid.front, 0, 0x40, 0x52);
                F_1371C(r_vid.front, 0x13f, 0x40, 0x52);
                F_11A24(0, 0x40, 0x140);
                F_11A24(0, 0x91, 0x140);
                x_colors.c[3] = sv;
            }
            if (color == -1) {
                color = random_int(5);
                F_067CF(color);
            }
            x1 = ((unsigned)specn - (unsigned)g7d86n) / 10;
            y = (((specn->f4 >> 8) & 7) == 2) ? 8 : 0;
            F_181C8(7, x1, y, 0, 0, 0x86, 0, g_5030[(specn->f4 >> 4) & 3]);
            F_119D6((0x140 - F_10B48(g_2990)) / 2, 0x88, g_2990);
            F_120FE(-2);
            g_05d0 = 1;
            F_14294(0x18);
            g_05d0 = 0;
            F_11BD3(bm1, 0, 0x40);
            F_193D4();
        }
test:
        n = F_009CB();
    } while (n != -3);
done:
    F_00D58();
    if (a_7eaap.m7eb0)
        F_067CF(a_7eaap.m7eb0->f4);
    g_0284 = 0;
}
