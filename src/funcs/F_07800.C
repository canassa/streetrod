/* F_07800 @ 0000:7800 (linear 0x07800, 241 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_07800(void)
{
    struct Car *car;
    int sel;
    struct Node *tbl[142];
    char line[142];
    int y, cur;

    car = g_7eb0;
    if (car == 0) {
        F_03AB8(0x0efa);
        return;
    }
    F_076A7(4, tbl, line, 0);
    switch (F_142CF(4, 1, 0, g_49e0, line, 0)) {
    case -4:
        return;
    case -3:
        sel = F_14C8B();
        break;
    case 0x3ea:
        F_03AB8(0x0f4e);
        return;
    }
    y = g_4806[car->slot12->idx].f2;
    cur = g_4806[tbl[sel]->idx].f2;
    F_07741(F_077B7(tbl[sel]), &car->slot12);
    F_1B213(g_5030[y], g_5030[cur]);
    g_05fc += 0x444L;
}
