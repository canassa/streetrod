/* F_009CB @ 0000:09CB (linear 0x009CB, 909 bytes) */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static struct Bitmap far * near F_009B7(int n);

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
            g_colors = g_style->hilite;
            g_051c = F_009B7(2);
            F_13682(g_vid.back, 0x40, 0, 0xc0, 0xbe);
            g_0526 = g_052a = 0;
            g_78a2(g_051c, g_vid.back, (struct Blt *)&g_0522, 1);
            g_0526 = 0x40;
            g_052a = 0x100;
            g_78a2(g_051c, g_vid.back, (struct Blt *)&g_0522, 1);
            F_19438(1);
            hsave = g_vid.back->height;
            g_vid.back->height = 0xbe;
            y = 0;
            i = g_0536 ? g_7ebe : g_pages.start[g_pages.cur];
            next = g_pages.list + i;
            row7 = g_0f82;
            cntz = 0;
            g_7ebe = i;
            do {
                F_1377F(g_vid.back, 0x44, y + 1, 0xb8);
                y += 3;
                if (y > 0xbd)
                    break;
                cntz++;
                row7++;
                row7->y1 = y - 2;
                row7->f0 = 0xb;
                item = *next++;
                row7->f6 = item >= 0xcd8 ? 0 : i;
                g_78ae(g_vid.back, 0x44, y, str = g_2c02 + item);
                for (;;) {
                    while ((c6 = *(str += strlen(str) + 1)) == '+') {
                        str++;
                        g_78ae(g_vid.back, g_824c, y, str);
                    }
                    if (c6 < 0x20)
                        break;
                    y += 9;
                    if (y > 0xbd)
                        break;
                    g_78ae(g_vid.back, 0x44, y, str);
                }
                y += 9;
                row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
                if (i >= limit) {
                    i = 0;
                    next = g_pages.list + g_pages.start[0];
                }
            } while (y < 0xbd && ++i <= limit);
            g_7f84 = i;
            g_0536 = 1;
            row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
            if (row7->y2 < 0xbd)
                F_1377F(g_vid.back, 0x44, y + 1, 0xb8);
            row7++;
            while (row7->f6 >= 0) {
                row7->f0 = 0xe;
                row7++;
            }
            g_0520 = 0;
            g_vid.back->height = hsave;
            F_120FE(-1);
            if (g_vid.f16)
                F_1A92A(g_vid.back, g_vid.front, 0, 0, 0x13f, 0xbd);
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
            g_7f84 = g_7ebe - 1;
            if (g_7f84 < 1)
                g_7f84 = limit;
            diff = 0xbd;
            while ((diff -= 9 * g_2c01[g_pages.list[g_7f84]] + 3) > 0)
                if (--g_7f84 < 1)
                    g_7f84 = limit;
            g_7ebe = g_7f84;
            g_0520 = 1;
            continue;
        case -2:
            if (++g_pages.cur >= g_pages.n)
                g_pages.cur = 0;
            g_7ebe = g_7f84 + (y > 0xc1 ? 0 : 1);
            g_0520 = 1;
            continue;
        default:
            g_0520 = 1;
        }
        return key;
    }
}
