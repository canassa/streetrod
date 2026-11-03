/* F_1929A: give back size bytes to the arena, top of the g_7b14 marker stack */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1929A(int size)
{
    g_8b02[g_7b14] -= size;
}
