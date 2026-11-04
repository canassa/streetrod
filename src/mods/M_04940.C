#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

char *g_4fa4 = g_3bf9;   /* DS:4FA4 */
int g_4fa6[14] = {26708, 8293, 24935, 25965, 27936, 29557, 8308, 24936, 25974, 24864, 28192, 28001, 8293, 33};   /* DS:4FA6 */

/* F_04940 @ 0000:4940 (linear 0x04940, 98 bytes) */


static int near F_04940(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_4fa4, "How about $%-d ?", *price);
    F_11DE2(0xff7, -1, -1);
    key = F_14294(0xd);
    F_11E5F();
    return key;
}

/* F_049A2 @ 0000:49A2 (linear 0x049A2, 75 bytes) */


int F_049A2(struct Car *c)
{
    int r;

    r = 0;
    if (c->f24 & 0x2000)
        r |= 4;
    if (c->f24 & 0x4000)
        r |= 2;
    if (c->f24 & 0x8000)
        r |= 1;
    if (g_4806[c->rack.s[0]->idx].f2 == 2)
        r |= 8;
    return r;
}
