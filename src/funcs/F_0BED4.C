/* F_0BED4 @ 0000:BED4 (linear 0x0BED4, 192 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static struct Node * near F_03FF0(int, int);
static int near F_04386(struct Node *, int);
static void near F_076A7(int, struct Node **, char *, int);
static void near F_07741(struct Node *, struct Node **);
static struct Node * near F_077B7(struct Node *);

static int near F_0BED4(void)
{
    struct Node *tbl[142];
    int ret;

    ret = 0;
    if (g_7eb0 == 0)
        return 0;
    g_7eb0->weight -= F_04386(g_7eb0->slot12, 0);
    g_7eb0->slot12 = 0;
    F_076A7(4, tbl, 0, 0);
    if (g_49e0[0]) {
        F_07741(F_077B7(tbl[1]), &g_7eb0->slot12);
    } else {
        g_7eaa -= 0x32;
        if (g_7eaa < 0) {
            ret = (int)-g_7eaa;
            g_7eaa = 0;
        }
        F_03FF0(0x28, g_4946);
        tbl[0] = g_7eb8;
        g_7eb8 = tbl[0]->next;
        F_07741(tbl[0], &g_7eb0->slot12);
    }
    return ret;
}
