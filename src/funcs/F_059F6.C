/* F_059F6 @ 0000:59F6 (linear 0x059F6, 340 bytes) */

#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_059F6(int create)
{
    int fd7;
    char hdr3[36];
    char name[20];
    int slot;
    int cnt;
    unsigned n;

    name[0] = F_15FBD();
    strcpy(name + 1, ":HOTROD");
    for (slot = 1, cnt = 0; slot <= 15; slot++) {
        itoa(slot, name + 8, 10);
        strcat(name, ".SAV");
        if (_dos_open(name, 0, &fd7)) {
            g_4fc2[slot] = 0;
        } else {
            if (_dos_read(fd7, hdr3, 0x24, &n) != 0 || n != 0x24)
                g_4fc2[slot] = 0;
            else {
                g_4fc2[slot] = slot;
                strncpy(g_2c02 + (g_49e0[++cnt] = g_4fd2[slot]), hdr3, 15);
            }
            _dos_close(fd7);
        }
        if (create && g_4fc2[slot] == 0) {
            strcpy(g_2c02 + (g_49e0[++cnt] = g_4fd2[slot]), g_4607);
        }
    }
    g_49e0[0] = cnt;
}
