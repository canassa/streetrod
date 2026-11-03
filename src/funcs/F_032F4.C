/* insert_entry @ 0000:32F4 (linear 0x032F4, 129 bytes) */

#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"



static void near insert_entry(int pos)
{
    int cnt;
    char *src;
    char *dst;

    cnt = g_count - pos - 1;
    if (pos >= 0) {
        src = (char *)&g_table[pos];
        if (cnt > 0) {
            dst = src + sizeof(struct Entry);
            memmove(dst, src, cnt * sizeof(struct Entry));
        }
        strcpy(src, g_newName);
        src += 16;
        itoa(g_score, src, 10);
    }
}
