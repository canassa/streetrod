/* F_27CAD @ 2645:185D (linear 0x27CAD, 158 bytes)

   Resets the terrain scroll state for a new track. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_27CAD(void)
{
    if (g_8aca)
        g_7d30 = 200;
    else
        switch (g_0284) {
        case 1:
            g_7d30 = 10;
            break;
        case 2:
            g_7d30 = 65;
            break;
        case 3:
            g_7d30 = 115;
            break;
        }
    F_27B6D(g_7d30);
    g_7d82 = 1500;
    g_8bd0 = 3300;
    g_7d32 = 0;
    g_7d36 = 2560;
    g_7d38 = 1900;
    g_7d3a = 0;
    g_7d3e = 0;
    g_7d3c = 0;
    g_7d34 = 0;
    g_54d4 = 1;
    g_8648 = !random_int(2);
    g_7d7e = 1;
}
