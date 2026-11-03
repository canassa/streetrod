/* F_07EE6 @ 0000:7EE6 (linear 0x07EE6, 203 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_07EE6(struct Car *car, int flag)
{
    int n, i;

    if (car->rack.s[0] == 0 || car->f1f <= 0 || car->rack.s[1] == 0)
        return 0;
    n = g_5126[g_4806[car->rack.s[1]->idx].f2];
    for (i = 2; i < n + 2; i++)
        if (car->rack.s[i] == 0)
            return 0;
    for (i = n * 2 + 3; --i >= 0; )
        if (car->b16[i] != 3)
            return 0;
    if (flag == 0)
        return 1;
    if (car->slot6 == 0)
        return 0;
    if (car->slot12 == 0)
        return 0;
    for (i = 2; --i >= 0; )
        if (car->b14[i] != 3)
            return 0;
    return 1;
}
