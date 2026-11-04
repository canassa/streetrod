#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int g7f84q;

static struct Bitmap far * near F_009B7(int n);

struct Pages g_pages = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};   /* DS:04FE */
struct Bitmap far *g_051c = 0;   /* DS:051C */
char g_0520 = 0;   /* DS:0520 */
int g_0522 = 0;   /* DS:0522 */
int g_0524 = 0;   /* DS:0524 */
int g_0526 = 0;   /* DS:0526 */
int g_0528 = 0;   /* DS:0528 */
int g_052a = 0;   /* DS:052A */
int g_052c = 0;   /* DS:052C */
int g_052e = 0;   /* DS:052E */
int g_0530[3] = {0, 0, 0};   /* DS:0530 */
int g_0536 = 0;   /* DS:0536 */

/* F_008B4 @ 0000:08B4 (linear 0x008B4, 259 bytes) */


void F_008B4(int *list, int base)
{
    int num;
    int pos;
    int *ptr;
    int first;

    g_0536 = 0;
    F_193D4();
    g_pages.list = list;
    g_pages.n = 0;
    ptr = list + 1;
    pos = 0xbe;
    num = first = *list;
    base = num - base;
    g_pages.cur = 1;
    while (num--) {
        while ((pos += 9 * g_2c01[*ptr] + 3) > 0xbd) {
            g_pages.start[g_pages.n++] = first - num;
            pos = 0;
        }
        if (num == base)
            g_pages.cur = g_pages.n - 1;
        ptr++;
    }
    x_colors = bustyle->hilite;
    F_120FE(-3);
    g_0522 = 0x40;
    g_0524 = 0xbe;
    g_0526 = g_0528 = g_052a = g_052c = g_052e = 0;
    g_0520 = 1;
    F_13DE6(8);
    F_120FE(-1);
}

/* F_009B7 @ 0000:09B7 (linear 0x009B7, 20 bytes) */


static struct Bitmap far * near F_009B7(int n)
{
    return F_15BBE(0x403, n);
}

/* F_009CB @ 0000:09CB (linear 0x009CB, 909 bytes) */



int F_009CB(void)
{
    int hsave;
    int key;
    char c6;
    int y;
    int i;
    int diff;
    char *str;
    int limit;
    int cntz;
    struct Hot *row7;
    int *next;
    int item;

    limit = *g_pages.list;
    for (;;) {
        if (g_0520) {
            F_120FE(-4);
            x_colors = bustyle->hilite;
            g_051c = F_009B7(2);
            F_13682(r_vid.back, 0x40, 0, 0xc0, 0xbe);
            g_0526 = g_052a = 0;
            b_78a2(g_051c, r_vid.back, (struct Blt *)&g_0522, 1);
            g_0526 = 0x40;
            g_052a = 0x100;
            b_78a2(g_051c, r_vid.back, (struct Blt *)&g_0522, 1);
            F_19438(1);
            hsave = r_vid.back->height;
            r_vid.back->height = 0xbe;
            y = 0;
            i = g_0536 ? a_7eaap.m7ebe : g_pages.start[g_pages.cur];
            next = g_pages.list + i;
            row7 = g_0f82;
            cntz = 0;
            a_7eaap.m7ebe = i;
            do {
                F_1377F(r_vid.back, 0x44, y + 1, 0xb8);
                y += 3;
                if (y > 0xbd)
                    break;
                cntz++;
                row7++;
                row7->y1 = y - 2;
                row7->f0 = 0xb;
                item = *next++;
                row7->f6 = item >= 0xcd8 ? 0 : i;
                a_78aem(r_vid.back, 0x44, y, str = g_2c02 + item);
                for (;;) {
                    while ((c6 = *(str += strlen(str) + 1)) == '+') {
                        str++;
                        a_78aem(r_vid.back, g824cd, y, str);
                    }
                    if (c6 < 0x20)
                        break;
                    y += 9;
                    if (y > 0xbd)
                        break;
                    a_78aem(r_vid.back, 0x44, y, str);
                }
                y += 9;
                row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
                if (i >= limit) {
                    i = 0;
                    next = g_pages.list + g_pages.start[0];
                }
            } while (y < 0xbd && ++i <= limit);
            g7f84q = i;
            g_0536 = 1;
            row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
            if (row7->y2 < 0xbd)
                F_1377F(r_vid.back, 0x44, y + 1, 0xb8);
            row7++;
            while (row7->f6 >= 0) {
                row7->f0 = 0xe;
                row7++;
            }
            g_0520 = 0;
            r_vid.back->height = hsave;
            F_120FE(-1);
            if (r_vid.f16)
                F_1A92A(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xbd);
            F_00D78();
        }
        while ((key = F_01417(3000)) == 0)
            ;
        switch (key) {
        case -3:
            break;
        case -1:
            if (--g_pages.cur < 0)
                g_pages.cur = g_pages.n - 1;
            g7f84q = a_7eaap.m7ebe - 1;
            if (g7f84q < 1)
                g7f84q = limit;
            diff = 0xbd;
            while ((diff -= 9 * g_2c01[g_pages.list[g7f84q]] + 3) > 0)
                if (--g7f84q < 1)
                    g7f84q = limit;
            a_7eaap.m7ebe = g7f84q;
            g_0520 = 1;
            continue;
        case -2:
            if (++g_pages.cur >= g_pages.n)
                g_pages.cur = 0;
            a_7eaap.m7ebe = g7f84q + (y > 0xc1 ? 0 : 1);
            g_0520 = 1;
            continue;
        default:
            g_0520 = 1;
        }
        return key;
    }
}

/* F_00D58 @ 0000:0D58 (linear 0x00D58, 32 bytes) */


void F_00D58(void)
{
    F_193D4();
    g_pages.list = 0;
    g_051c = 0;
    F_13F21(1);
}
