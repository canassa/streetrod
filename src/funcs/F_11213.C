#include "GLOBALS.H"
#include "PROTOS.H"

int F_11213(struct Obj *o, struct Cmd *c)
{
    int old;

    switch (o->id) {
    case 2:
        if (c->a) {
            g_6c3e = c->a;
            F_16993(1);
            return 0;
        }
        if (g_6c3e--) {
            int t1, t2, t3;
            old = g_colors.c[3];
            g_colors.c[3] = (g_6c3e & 1) ? 4 : 0;
            F_1377F(g_vid.back, 0x33, 0x6f, 2);
            if (!(g_6c3e & 3)) {
                g_colors.c[3] = (g_6c3e & 4) ? 4 : 0;
                F_1377F(g_vid.back, 0x30, 0x6f, 2);
            }
            return 0;
        }
        F_16993(0);
        return 1;
    }
}
