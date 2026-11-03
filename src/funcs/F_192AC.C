/* F_192AC: allocate size bytes from the arena at g_6cd0; mode picks which of four marker stacks is used */

#include "GLOBALS.H"
#include "PROTOS.H"

char far *F_192AC(int size, int mode)
{
    char far *p;

    p = g_6cd0;
    switch (mode) {
    case 0:
        p += g_8f00;
        g_8f00 += size;
        g_8b02[0] = g_8f00;
        break;
    case 1:
        p += g_8b02[g_7b14];
        g_8b02[g_7b14 + 1] = size + g_8b02[g_7b14];
        g_7b14++;
        break;
    case 2:
        g_8b02[g_7d80] -= size;
        p += g_8b02[g_7d80];
        g_8b02[g_7d80 - 1] = g_8b02[g_7d80] - 1;
        g_7d80--;
        break;
    case 3:
        g_78de -= size;
        p += g_78de--;
        g_8b02[60] = g_78de;
        break;
    }
    return p;
}
