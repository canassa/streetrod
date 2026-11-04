#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
void (far *a_78aem)(struct Bitmap far *, int, int, char *);
struct State7EAA a_7eaap;
int g824cd;
int g8258z;
int ar82c0;

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

void F_11669(int i)
{
    int saved;

    saved = aq8256z;
    aq8256z = 1;
    F_11421(g8258z, g_msgs[i], 8);
    aq8256z = saved;
}
