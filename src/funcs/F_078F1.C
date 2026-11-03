/* F_078F1 @ 0000:78F1 (linear 0x078F1, 250 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_078F1(int *ids, char *vals)
{
    int n;
    struct Item *it;
    int count;
    struct Node *node;
    struct Node *gear[7];

    for (n = 0; n < 5; n++)
        gear[n] = g_7eb0->rack.s[n];
    gear[n++] = g_7eb0->slot6;
    gear[n] = g_7eb0->slot12;

    for (count = 0, n = 0; n < 7; n++) {
        if (node = gear[n]) {
            count++;
            it = &g_4806[node->idx];
            ids[count] = g_50ce[n];
            g_49e0[count] = it->f6;
            if (node->b != -0x80)
                vals[count] = node->b / 100;
            else
                vals[count] = 0x80;
        }
    }
    for (n = 1; n <= 4; n++) {
        g_49e0[++count] = -n;
        vals[count] = 0x80;
        ids[count] = 0;
    }
    g_49e0[0] = count;
}
