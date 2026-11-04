#include <string.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "M_02FC8.H"
#include "M_06476.H"

static int g_score;                        /* DS:6C5E */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int bucount;
char atnewName[16];

/* insert_entry @ 0000:32F4 (linear 0x032F4, 129 bytes) */




static void near insert_entry(int pos)
{
    int cnt;
    char *src;
    char *dst;

    cnt = bucount - pos - 1;
    if (pos >= 0) {
        src = (char *)&aytablex[pos];
        if (cnt > 0) {
            dst = src + sizeof(struct Entry);
            memmove(dst, src, cnt * sizeof(struct Entry));
        }
        strcpy(src, atnewName);
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
