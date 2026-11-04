/* TANDYLIN.H - declarations of what TANDYLIN.ASM defines. */

#ifndef TANDYLIN_H
#define TANDYLIN_H
#include "STRUCTS.H"

void far tl_draw();
void tl_span(int x1, int x2, int y, int color);
void far tl_fill_below();

#endif
