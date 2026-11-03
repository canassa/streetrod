#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_10DDF(void)
{
    switch (g_cmd->sub) {
    case 0:
        break;
    case 1:
        g_box->x1 = g_cmd->a;
        g_box->y1 = g_cmd->b;
        break;
    case 3:
        g_box->x1 = g_cmd->a;
        g_box->y1 = g_cmd->b;
    case 2:
        g_box->x2 = g_cmd->a;
        g_box->y2 = g_cmd->b;
        break;
    }
}
