#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>
#include <conio.h>
#include <stdio.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
char af824ax;

void F_1169A(int x, int y, int w, int h, char color)
{
    char old;

    old = x_colors.c[2];
    F_120FE(-3);
    x_colors.c[2] = color;
    F_13682(r_vid.back, x, y, w, h);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, x + w - 1, y + h - 1);
    x_colors.c[2] = old;
    F_120FE(-1);
}

void F_1171C(char *s)
{
    strcpy(g_291f, s);
    F_14241(0x1d);
}

char g_04ac[18] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14, 0, 0};   /* DS:04AC */
int g_04be = 0;   /* DS:04BE */
int g_04c0 = 0;   /* DS:04C0 */
int g_04c2 = 190;   /* DS:04C2 */

/* F_1174E: show a message and wait for a key ("Hit Return when ready ...") */


void F_1174E(char *msg)
{
    int saved;
    char prev;
    int dx;
    int i;
    struct Bitmap far *back;

    saved = r_vid.f16;
    strcpy(g_293e, msg);
    strcpy(g_2967, "Hit Return when ready ...");
    if (ap8247u >= 1) {
        if (g_58ee) {
            F_1169A(0, 0, 320, 200, 0);
            g_58ee++;
        }
        prev = gz8249;
        if (!prev) {
            gz8249 = 1;
            (*m_present)();
        }
        dx = af824ax ? 5 : 0x50;
        for (i = 0; i < 5; i++) {
            g_1d8a[i].x1 += dx;
            g_1d8a[i].x2 += dx;
        }
        if (g_58ee)
            F_11324(g_04ac);
        r_vid.f16 = 1;
        F_00E24();
        back = r_vid.back;
        r_vid.back = r_vid.front;
        F_14241(0x20);
        r_vid.back = back;
        for (i = 0; i < 5; i++) {
            g_1d8a[i].x1 -= dx;
            g_1d8a[i].x2 -= dx;
        }
        r_vid.f16 = saved;
        gz8249 = prev;
        if (!prev)
            (*bl78be)();
    } else {
        puts(" ");
        puts(g_293e);
        puts(g_2967);
        if (g_05e0) {
            while (!F_02DB6())
                ;
        } else {
            while (!kbhit() == 0)
                ;
            getch();
        }
    }
}

/* F_118D4: blit bitmap number a to the back buffer at (g_04be, g_04c0), then copy the rectangle to the front
   buffer. b >= 0 first fills the screen with color b. */


int F_118D4(int a, int b)
{
    struct Bitmap far *p;
    long unused;
    struct Blt pkt;

    F_120FE(-3);
    p = F_15BBE(a, 2);
    if (b >= 0)
        F_1169A(0, 0, 320, g_04c2, b);
    pkt.w = p->width;
    pkt.h = p->height > g_04c2 ? g_04c2 : p->height;
    pkt.sx = pkt.sy = pkt.fc = 0;
    pkt.dx = g_04be;
    pkt.dy = g_04c0;
    (*b_78a2)(p, r_vid.back, &pkt, 1);
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, g_04be, g_04c0, pkt.dx + pkt.w - 1, pkt.dy + pkt.h - 1);
    g_04be = g_04c0 = 0;
    F_19438(1);
}

/* F_119B8 @ 0F38:2638 (linear 0x119B8, 30 bytes) */


int F_119B8(int a, int b, int c, int d)
{
    g_04be = b;
    g_04c0 = c;
    return F_118D4(a, d);
}
