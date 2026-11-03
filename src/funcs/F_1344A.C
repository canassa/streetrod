/* F_1344A: build the sorted list of menu items in g_184c: the 3 fixed records are copied from g_1948,
   then for each of 9 slots (grouped by the counts in g_5130) that is enabled in g_8290 an item is
   inserted at the position F_12964 gives for the slot's coordinates, plus a last item picked by
   g_82ac. g_82ae/g_82b2 are saved and restored around it. */

#include <string.h>
#pragma intrinsic(memcpy)
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1287B(void);
static void near F_133CB(struct Link *arr, struct Link *item, int num);

void F_1344A(int *xs, int *ys)
{
    int lane;
    int num;
    int x, i, y;
    int rh, rx;
    int j;
    struct Link *item;
    int len;

    rx = g_82ae;
    rh = g_82b2;
    item = g_1948;
    len = (num = 3) * sizeof(struct Link);
    memcpy(g_184c, item, len);
    g_0796.f4 = g_2ba5;
    i = lane = j = 0;
    for (; i < 9; i++) {
        if (g_8290[lane]) {
            g_82ae = xs[i];
            g_82b2 = ys[i];
            F_12964(&x, &y);
            x += 0x60;
            y += 9;
            g_0796.f6 = i + 1;
            g_0796.fc = (g_0796.f8 = x - 2) + 5;
            g_0796.fa = (g_0796.fe = y + 2) - 15;
            F_133CB(g_184c, (struct Link *)&g_0796, num);
            num++;
        }
        if (++j == g_5130[lane]) {
            lane++;
            j = 0;
        }
    }
    if (g_8290[0]) {
        g_0796.f4 = g_2ba7;
        g_0796.f6 = 13;
        i = (g_82ac == 0x446) + F_1287B() * 2;
        g_0796.fc = (g_0796.f8 = g_071a[i * 2] + 0x5b) + 10;
        g_0796.fe = (g_0796.fa = g_071a[i * 2 + 1] + 2) + 15;
        F_133CB(g_184c, (struct Link *)&g_0796, num);
        num++;
    }
    g_184c[num].nxt = 9;
    g_82ae = rx;
    g_82b2 = rh;
    F_00D78();
}
