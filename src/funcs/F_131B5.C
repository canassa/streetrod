/* F_131B5: draw the item pictures of a car's parts for slots 0, 1 and then 2..4, with the
   screen shadow off (g_06fc). Loads the g_vid.f8 strip first when it isn't there yet. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_131B5(struct Car *car)
{
    int mode;
    int slot;
    int type;
    int other;

    mode = g_82ae;
    g_06fc = 0;
    if (g_vid.f8 == 0)
        g_vid.f8 = F_15BBE(0x42d, 1);
    else
        F_1592E(0x42d, g_vid.f8);
    if (type = g_8290[0]) {
        g_82ae = 0;
        F_12DD8(type, 0);
        if (type == 0x434)
            g_078c[0] = 4;
        else
            g_078c[0] = 5;
        if (type = g_8290[1]) {
            g_82ae = 1;
            F_12DD8(type, 0);
            slot = g_078c[g_4808[car->rack.s[1]->idx].kind];
            for (type = 0; type < 3; type++) {
                g_82ae = slot++;
                if (other = g_8290[type + 2])
                    F_12DD8(other, 0);
            }
        }
    }
    g_06fc = 1;
    g_82ae = mode;
}
