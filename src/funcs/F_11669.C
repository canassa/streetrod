#include "GLOBALS.H"
#include "PROTOS.H"

void F_11669(int i)
{
    int saved;

    saved = g_8256;
    g_8256 = 1;
    F_11421(g_8258, g_msgs[i], 8);
    g_8256 = saved;
}
