#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

int g_4bca[10] = {59, 70, 81, 92, 103, 114, 125, 136, 147, 158};   /* DS:4BCA */

/* insert_entry @ 0000:32F4 (linear 0x032F4, 129 bytes) */




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

/* score_index @ 0000:3375 (linear 0x03375, 42 bytes) */


static int near score_index(void)
{
    int a;
    int b;

    F_064A3(&a, &b);
    return F_06476() * 24 + a + 12;
}
