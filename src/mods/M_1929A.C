#include "GLOBALS.H"
#include "PROTOS.H"

/* F_1929A: give back size bytes to the arena, top of the g7b14o marker stack */


void F_1929A(int size)
{
    c_8b02[g7b14o] -= size;
}
