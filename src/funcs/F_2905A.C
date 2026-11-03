/* F_2905A @ 2645:2C0A (linear 0x2905A, 242 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_2905A(int x, int y, int first, struct List l, int dir)
{
    struct Run run;
    int px;
    int x2;
    int y1;
    int j;

    if (l.last > first) {
        run = l.items[first];
        if (dir == -1) {
            for (j = 0; j < run.n; j++) {
                px = x - run.parts[j].dx;
                x2 = px - run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_78d6)(x2, px, y1, run.parts[j].h);
            }
        } else {
            for (j = 0; j < run.n; j++) {
                px = run.parts[j].dx + x;
                x2 = px + run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_78d6)(px, x2, y1, run.parts[j].h);
            }
        }
    }
}
