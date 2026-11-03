/* F_0A35C @ 0000:A35C (linear 0x0A35C, 186 bytes) - blit sprite-sheet cell n (7 per row) of bm into dst */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0A35C(struct Bitmap far *bm, int n, struct Bitmap far *dst)
{
    struct Blt cell;
    int row;
    struct Blt bl;
    int col;

    if (n < 14) {
        row = n / 7;
        col = n - 7 * row;
    } else {
        row = 0;
        col = 0;
    }
    cell.w = 40;
    cell.h = 50;
    cell.sx = 40 * col;
    cell.sy = 50 * row;
    cell.dx = 184;
    cell.dy = 73;
    if (g_0284 == 6) {
        bl.w = 16;
        bl.h = 50;
        bl.sx = 208;
        bl.sy = 73;
        bl.dx = cell.sx + 24;
        bl.dy = cell.sy;
        (*g_78a2)(g_vid.front, bm, &bl, 1);
    }
    (*g_78a2)(bm, dst, &cell, 1);
}
