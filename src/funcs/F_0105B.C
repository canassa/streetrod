/* F_0105B @ 0000:105B (linear 0x0105B, 182 bytes) */

#include <dos.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_0105B(unsigned deverr, unsigned char errcode)
{
    int disk;
    int action;

    disk = (deverr & 0x8000) == 0;
    if (disk) {
        if (deverr & 0x1000)
            action = 1;
        else
            action = 2;
    } else
        action = 1;
    switch (errcode) {
    case 0:
        g_060e[5] = deverr + 'A';
        F_1174E(g_060e);
        break;
    case 2:
        g_060c[strlen(g_060c) - 1] = deverr + 'A';
        F_1174E(g_060c);
        break;
    case 9:
        F_1174E("printer out of paper");
        break;
    default:
        if (disk)
            F_1174E("disk error");
        else {
            F_1174E("hardware error");
            action = 2;
        }
    }
    if (action == 2)
        F_00FEA();
    _hardresume(action);
}
