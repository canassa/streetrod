#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include "STRUCTS.H"
#include "M_113A2.H"
#include "HOTSPOT.H"
#include "MESSAGES.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02DB6.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_11E9C.H"
#include "M_13682.H"
#include "M_139DE.H"
#include "M_15396.H"
#include "M_174A6.H"
#include "M_193D4.H"
#include "M_19ED0.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (CLAUDE.md 4e). */
int o_8250;
int au8252x;
int as8254y;
int aq8256z;

char *g_msgs[7] = {"0", "1", "2", "3", "4", "N", "D"};   /* DS:0482 */

/* F_113A2 @ 0F38:2022 (linear 0x113A2, 127 bytes) */

void F_113A2(int y)
{
    au8252x = y + 1;
    as8254y = 0xa0;
    o_8250 = 0x58;
    aq8256z = 0;
    x_colors = bustyle->colors;
    F_13682(r_vid.back, 0, y, 320, 10);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, 0, y, 319, y + 9);
}

void (far *a_78aem)(struct Bitmap far *, int, int, char *);
struct State7EAA a_7eaap;
int g824cd;
int g8258z;
int ar82c0;

/* F_11421 @ 0F38:20A1 (linear 0x11421, 287 bytes) */

void F_11421(int x, char *s, int w)
{
    struct Colors saved;
    int left;
    int wd;
    int ypos;

    saved = x_colors;
    x_colors = bustyle->colors;
    if (w == 0)
        w = as8254y;
    if (x < 0) {
        x = o_8250;
        left = (x + (w - F_10B48(s)) / 2) & 0xfff8;
    } else
        left = x;
    wd = 320 - x;
    ypos = au8252x;
    F_120FE(-3);
    if (aq8256z)
        F_13682(r_vid.back, x - 1, ypos, wd, 8);
    (*a_78aem)(r_vid.back, left, ypos, s);
    aq8256z = *s;
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x > 8 ? x - 8 : x, ypos, x + wd, ypos + 7);
    x_colors = saved;
}

/* F_11540 @ 0F38:21C0 (linear 0x11540, 83 bytes) */

void F_11540(int n)
{
    int old;
    char buf[10];

    old = aq8256z;
    aq8256z = 1;
    F_11421(g8258z, ltoa(a_7eaap.m7eaa += n, buf, 10), 0x30);
    ar82c0 = g824cd;
    aq8256z = old;
}

/* F_11593 @ 0F38:2213 (linear 0x11593, 214 bytes) */

void F_11593(char *s)
{
    int saved;

    saved = r_vid.f16;
    x_colors = bustyle->colors;
    F_120FE(-3);
    F_13682(r_vid.back, 0, au8252x, o_8250 & 0xfff8, 8);
    (*a_78aem)(r_vid.back, 0, au8252x, s);
    g8258z = (g824cd + 7) & 0xfff8;
    r_vid.f16 = 0;
    if (s[1] != 'G' && s[1] != ' ')
        F_11540(0);
    r_vid.f16 = saved;
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, 0, au8252x, o_8250, au8252x + 7);
}

/* F_11669 @ 0F38:22E9 (linear 0x11669, 49 bytes) */

void F_11669(int i)
{
    int saved;

    saved = aq8256z;
    aq8256z = 1;
    F_11421(g8258z, g_msgs[i], 8);
    aq8256z = saved;
}

char af824ax;

/* F_1169A @ 0F38:231A (linear 0x1169A, 130 bytes) */

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

/* F_1171C @ 0F38:239C (linear 0x1171C, 50 bytes) */

void F_1171C(char *s)
{
    strcpy(g_291f, s);
    F_14241(0x1d);
}

char g_04ac[18] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14, 0, 0};   /* DS:04AC */
int g_04be = 0;   /* DS:04BE */
int g_04c0 = 0;   /* DS:04C0 */
int g_04c2 = 190;   /* DS:04C2 */

/* F_1174E @ 0F38:23CE (linear 0x1174E, 390 bytes)

   Show a message and wait for a key ("Hit Return when ready ...") */

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

/* F_118D4 @ 0F38:2554 (linear 0x118D4, 228 bytes)

   Blit bitmap number a to the back buffer at (g_04be, g_04c0), then copy the rectangle to the front
   buffer. b >= 0 first fills the screen with color b. */

int F_118D4(int a, int b)
{
    struct Bitmap far *p;
    long unused;    /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
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

/* F_119D6 @ 0F38:2656 (linear 0x119D6, 78 bytes)

   Draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */

void F_119D6(int x, int y, char *s)
{
    (*a_78aem)(r_vid.back, x, y, s);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, g824cd - 1, y + 7);
}

/* F_11A24 @ 0F38:26A4 (linear 0x11A24, 78 bytes)

   Draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */

void F_11A24(int x, int y, int w)
{
    F_1377F(r_vid.back, x, y, w);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, x + w - 1, y);
}
