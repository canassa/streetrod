/* F_0A265 @ 0000:A265 (linear 0x0A265, 247 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Car * near F_04036(int idx, int weight, struct Car *old);
static void near F_081AB(struct Car *c);

static void near F_0A265(void)
{
    struct Bitmap far *pict;
    struct Car chassis;
    int n;
    struct Node nodes[5];
    struct Node n1;
    struct Node na;

    F_193FE();
    pict = F_11A72(0x60, 9, 200, 143, 0);
    chassis.slot6 = &n1;
    chassis.slot12 = &na;
    for (n = 0; n < 5; n++)
        chassis.rack.s[n] = &nodes[n];
    F_04036(g_7648, 0, &chassis);
    F_081AB(&chassis);
    F_11421(-1, "", 0);
    F_13DE6(0x18);
    g_05cc = g_tick + 800;
    while ((n = F_01417(3000)) == 0)
        ;
    g_vid.f8 = 0;
    F_11BD3(pict, 0x60, 9);
    F_193FE();
    F_13F21(1);
}
